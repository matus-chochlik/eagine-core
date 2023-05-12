#!/usr/bin/env python3
# coding=utf-8
# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
#
import os
import re
import sys
import glob
import json
import stat
import time
import errno
import random
import signal
import socket
import logging
import xml.sax
import argparse
import tempfile
import subprocess

try:
    import selectors
except ImportError:
    import selectors2 as selectors

# ------------------------------------------------------------------------------
#  Argument parsing
# ------------------------------------------------------------------------------
class ArgumentParser(argparse.ArgumentParser):
    # --------------------------------------------------------------------------
    def userConfigPath(self, arg):
        return os.path.join(
            os.path.expanduser("~"),
            ".config",
            "eagine",
            arg)

    # --------------------------------------------------------------------------
    def configSearchPaths(self, arg):
        return [
            os.path.abspath(arg),
            self.userConfigPath(arg)
        ]

    # --------------------------------------------------------------------------
    def configSearchExts(self):
        return [".eagiproc", ".json"]

    # --------------------------------------------------------------------------
    def findConfigPath(self, arg):
        for path in self.configSearchPaths(arg):
            for ext in self.configSearchExts():
                tmp = path + ext
                if os.path.isfile(tmp):
                    return tmp

    # --------------------------------------------------------------------------
    def configBasename(self, path):
        return os.path.splitext(os.path.basename(path))[0]

    # --------------------------------------------------------------------------
    def findConfigNames(self):
        result = []
        for path in self.configSearchPaths(os.path.curdir):
            for ext in self.configSearchExts():
                for filepath in glob.glob(os.path.join(path, "*"+ext)):
                    yield filepath

    # --------------------------------------------------------------------------
    def __init__(self, **kw):
        argparse.ArgumentParser.__init__(self, **kw)

        def config_path(arg):
            result = self.findConfigPath(arg)
            return result if result else arg

        self.add_argument(
            dest="config_paths",
            metavar="config",
            type=config_path,
            nargs='*',
            help="""
                Specifies the path to or the name of a configuration file.
                Configuration files are read in the order specified on
                the command line. Variables in later files replace variables
                from earlier files. If config is not a valid filesystem path,
                then following paths `%(paths)s', are searched for files with
                names specified in config, with extensions: `%(exts)s'.
                Currently visible configs: `%(configs)s'.
            """ % {
                "paths": "', `".join(self.configSearchPaths("config")),
                "exts": "', `".join(self.configSearchExts()),
                "configs": "', `".join([self.configBasename(x) for x in self.findConfigNames()])
            }
        )

        def key_value(arg):
            sep = '='
            tmp = arg.split(sep)
            return (tmp[0], sep.join(tmp[1:]))

        self.add_argument(
            "--set", "-S",
            dest="overrides",
            metavar="variable=value",
            type=key_value,
            action="append",
            default=[],
            help="""
                Specifies new values for the config variables.
                Variables from loaded configuration files are always overriden,
                by values specified on the command-line.
            """
        )

        self.add_argument(
            "--dry-run",
            action="store_true",
            default=False,
            help="""Does not actually start anything just prints selected information."""
        )

        self.add_argument(
            "--print-config", "-P",
            action="store_true",
            default=False,
            help="""Prints the fully loaded and merged process configuration."""
        )

    # --------------------------------------------------------------------------
    def processParsedOptions(self, options):
        return options

    # --------------------------------------------------------------------------
    def parseArgs(self):
        # ----------------------------------------------------------------------
        class _Options(object):
            # ------------------------------------------------------------------
            def __init__(self, base):
                self.__dict__.update(base.__dict__)
                self._hashes = set()
                self._named_hashes = dict()
                self._work_dir = tempfile.TemporaryDirectory(prefix="eagi")

            # ------------------------------------------------------------------
            def __del__(self):
                self._work_dir.cleanup()

            # ------------------------------------------------------------------
            def _genHash(self):
                return "%032x" % random.getrandbits(128)

            # ------------------------------------------------------------------
            def uniqueHash(self, key):
                if key is not None:
                    try:
                        return self._named_hashes[key]
                    except KeyError:
                        pass

                while True:
                    h = self._genHash()
                    if h not in self._hashes:
                        self._hashes.add(h)
                        if key is not None:
                            self._named_hashes[key] = h
                        return h

            # ------------------------------------------------------------------
            def workDir(self):
                return self._work_dir.name

            # ------------------------------------------------------------------
            def logSocketPath(self):
                return os.path.join(self.workDir(), "eagilog.socket")

        # ----------------------------------------------------------------------
        return _Options(self.processParsedOptions(
            argparse.ArgumentParser.parse_args(self)
        ))

# ------------------------------------------------------------------------------
def getArgumentParser():
    return ArgumentParser(
        prog=os.path.basename(__file__),
        description="""launches and manages a group of processes"""
    )

# ------------------------------------------------------------------------------
keepRunning = True
# ------------------------------------------------------------------------------
#  Configuration parse utilities
# ------------------------------------------------------------------------------
class ExpansionRegExprs(object):
    # --------------------------------------------------------------------------
    def __init__(self, options):
        self._options = options
        self.commands = [
            ("pathid",
                re.compile(".*(\$\(pathid\s+([^)]+)\)).*"),
                lambda mtch : self._resolveCmdPathId(mtch.group(2))
            ),
            ("in_work_dir",
                re.compile(".*(\$\(in_work_dir\s+([^)]+)\)).*"),
                lambda mtch : self._resolveCmdInWorkDir(mtch.group(2))
            ),
            ("basename",
                re.compile(".*(\$\(basename\s+([^)]+)\)).*"),
                lambda mtch : self._resolveCmdBasename(mtch.group(2))
            ),
            ("dirname",
                re.compile(".*(\$\(dirname\s+([^)]+)\)).*"),
                lambda mtch : self._resolveCmdDirname(mtch.group(2))
            ),
            ("wildcard",
                re.compile(".*(\$\(wildcard\s+([^)]+)\)).*"),
                lambda mtch : self._resolveCmdWildcard(mtch.group(2))
            ),
            ("which",
                re.compile(".*(\$\(which\s+([^)]+)\)).*"),
                lambda mtch : self._resolveCmdWhich(mtch.group(2))
            ),
            ("eagiapp",
                re.compile(".*(\$\(eagiapp\s+([^)]+)\)).*"),
                lambda mtch : self._resolveCmdEAGiApp(mtch.group(2))
            ),
            ("range",
                re.compile(".*(\$\(range\s+([0-9]+)\s+([0-9]+)\)).*"),
                lambda mtch : self._resolveCmdRange(mtch.group(2), mtch.group(3))
            ),
            ("nuid",
                re.compile(".*(\$\(uid\s+([^)]+)\)).*"),
                lambda mtch : self._resolveCmdUid(mtch.group(2))
            ),
            ("uid",
                re.compile(".*(\$\(uid\)).*"),
                lambda mtch : self._resolveCmdUid(None)
            )
        ]

        self.adjustments = [
            ("identity",
                re.compile(".*(\$<identity ([^)]+)>).*"),
                lambda inst, mtch : self._resolveProcIdentity(mtch.group(2))
            ),
            ("timestamp",
                re.compile(".*(\$<timestamp>).*"),
                lambda inst, mtch: time.time()
            ),
            ("instance",
                re.compile(".*(\$<instance>).*"),
                lambda inst, mtch: inst.instanceIndex()
            ),
            ("uid",
                re.compile(".*(\$<uid>).*"),
                lambda inst, mtch: self._resolveCmdUid()
            )
        ]

        self.variable = re.compile(".*(\${([A-Za-z][A-Za-z_0-9]*)}).*")
        self.list_exp = re.compile(".*(\$\[([A-Za-z][A-Za-z_0-9]*)\.\.\.\]).*")
        self.sbsc_exp = re.compile(".*(\$\[([A-Za-z][A-Za-z_0-9]*)\[([0-9]+)\]\]).*")
        self.eval_exp = re.compile(".*(\$\(([0-9+*/%-]*)\)).*")

    # --------------------------------------------------------------------------
    def _resolveCmdInWorkDir(self, name):
        return os.path.join(self._options.workDir(), name)

    # --------------------------------------------------------------------------
    def _resolveCmdBasename(self, name):
        return os.path.basename(name)

    # --------------------------------------------------------------------------
    def _resolveCmdDirname(self, name):
        return os.path.dirname(name)

    # --------------------------------------------------------------------------
    def _resolveCmdPathId(self, name):
        return int(hash(name))

    # --------------------------------------------------------------------------
    def _resolveCmdWildcard(self, pattern):
        return [os.path.realpath(x) for x in glob.glob(pattern)]

    # --------------------------------------------------------------------------
    def _resolveCmdWhich(self, name):
        search_dirs = os.environ.get("PATH", "").split(':')
        search_dirs += os.path.dirname(__file__)
        for dir_path in search_dirs:
            cmd_path = os.path.join(dir_path, name)
            if os.path.isfile(cmd_path):
                if stat.S_IXUSR & os.stat(cmd_path)[stat.ST_MODE]:
                    return cmd_path
        return name

    # --------------------------------------------------------------------------
    def _resolveProcIdentity(self, expr):
        return ["--log-identity", expr]

    # --------------------------------------------------------------------------
    def _resolveCmdEAGiApp(self, name):
        file_dir = os.path.dirname(__file__)
        def _get_build_dir():
            bdf_path = os.path.join(file_dir, os.pardir, os.pardir, "BINARY_DIR")
            try:
                with open(bdf_path, "r") as bdf:
                    return os.path.realpath(os.path.join(
                            bdf.readline().strip(), os.pardir, os.pardir))
            except: pass

        def _scantree(what, path):
            try:
                for entry in os.scandir(path):
                    if os.path.basename(entry.path) == what:
                        if stat.S_IXUSR & os.stat(entry.path)[stat.ST_MODE]:
                            return entry.path
                    elif entry.is_dir(follow_symlinks=False):
                        found = _scantree(what, entry.path)
                        if found:
                            return found
            except: pass

        def _scanopts(path):
            prefixes = ["eagine", "app", "oglplus", "oalplus", "eglplus"]
            for option in [name] + [p+"-"+name for p in prefixes]:
                found = _scantree(option, path)
                if found:
                    return found

        def _search():
            yield _scanopts(file_dir)
            yield _scanopts(os.path.join(file_dir, os.pardir, "share", "eagine"))
            yield _scanopts(_get_build_dir())
            yield self._resolveCmdWildcard(name)

        for found in _search():
            if found:
                return [found, "--use-asio-log", self._options.logSocketPath()]

    # --------------------------------------------------------------------------
    def _resolveCmdRange(self, ifrom, ito):
        return [i for i in range(int(ifrom), int(ito)+1)]

    # --------------------------------------------------------------------------
    def _resolveCmdUid(self, key):
        return self._options.uniqueHash(key)

# ------------------------------------------------------------------------------
def mergeConfigs(source, destination):
    for key, value in source.items():
        if isinstance(value, dict):
            node = destination.setdefault(key, {})
            mergeConfigs(value, node)
        elif isinstance(value, list):
            node = destination.setdefault(key, [])
            node += value
        else:
            destination[key] = value

    return destination

# ------------------------------------------------------------------------------
#  Expectations
# ------------------------------------------------------------------------------
class ExpectCleanShutdown(object):
    # --------------------------------------------------------------------------
    def __init__(self, identity):
        self._satisfied = False
        self._identity = identity

    # --------------------------------------------------------------------------
    def update(self, arg):
        self._satisfied = arg

    # --------------------------------------------------------------------------
    def check(self):
        if self._satisfied:
            return True

        logging.error("unexpected failed shutdown of '%s'" % self._identity)
        return False

# ------------------------------------------------------------------------------
class ExpectExitCode(object):
    # --------------------------------------------------------------------------
    def __init__(self, identity, value):
        self._expected_value = value
        self._actual_values = []
        self._identity = identity

    # --------------------------------------------------------------------------
    def update(self, instance, value):
        self._actual_values.append(value)

    # --------------------------------------------------------------------------
    def check(self):
        actual = set(self._actual_values)
        if all(v == self._expected_value for v in actual):
            return True

        if len(actual) == 0:
            logging.error("did not receive exit notification of '%s'", self._identity) 

        for value in actual: 
            logging.error("unexpected exit code %d of '%s', expected: %d" % (
                value,
                self._identity,
                self._expected_value))

        return False

# ------------------------------------------------------------------------------
#  Configuration
# ------------------------------------------------------------------------------
class PipelineConfig(object):
    # --------------------------------------------------------------------------
    def __init__(self, options):
        self._res = ExpansionRegExprs(options)
        self._options = options
        self.full_config = {}

        if options.config_paths:
            for config_path in options.config_paths:
                implicit = {
                    "THIS_CFG": os.path.splitext(os.path.basename(config_path))[0],
                    "THIS_EXT": "".join(os.path.splitext(os.path.basename(config_path))[1:])
                }
                try:
                    with open(config_path) as config_file:
                        partial_config = json.load(config_file)
                        for proc in partial_config.get("pipelines", []):
                            try:
                                proc["variables"].update(implicit)
                            except KeyError:
                                proc["variables"] = implicit

                        self.full_config = mergeConfigs(
                            partial_config,
                            self.full_config)
                except IOError as io_error:
                    logging.error("reading '%s': %s" % (config_path, io_error))
                except ValueError as value_error:
                    logging.error("parsing '%s': %s" % (config_path, io_error))

        for pipeline in self.full_config.get("pipelines", []):
            variables = self.full_config.get("variables", {}).copy()
            for name, expr in pipeline.get("variables", {}).items():
                values = self.resolve(expr, variables)
                if len(values) == 1:
                    variables[name] = values[0]
                elif len(values) > 1:
                    variables[name] = values
            variables.update(options.overrides)
            pipeline["variables"] = variables

            if "commands" in pipeline:
                commands = pipeline["commands"]
                pipeline["commands"] = []
                for args in commands:
                    pipeline["commands"].append(self.resolve(args, variables))
            elif "command" in pipeline:
                args = pipeline["command"]
                pipeline["commands"] = [self.resolve(args, variables)]
                del pipeline["command"]

        if options.print_config:
            print(
                json.dumps(
                    self.full_config,
                    sort_keys=True,
                    indent=2,
                    separators=(', ', ': ')
                )
            )

    # --------------------------------------------------------------------------
    def _fallback(self, name):
        if name in ["WORK_DIR"]:
            return self._options.workDir()

        if name in ["HOME"]:
            return os.path.expanduser("~")

        if name in ["SELF"]:
            return os.path.abspath(__file__)

        return name

    # --------------------------------------------------------------------------
    def _doSubVars(self, expr, variables):
        if type(expr) is not str:
            yield expr
            return

        def _expandVar(name):
            return str(variables.get(name, os.environ.get(name, self._fallback(name))))

        value = _expandVar(expr)

        if re.match(self._res.variable, value):
            global keepRunning
            while keepRunning:
                found = re.match(self._res.variable, value)
                if found:
                    prev = value[:found.start(1)]
                    repl = _expandVar(found.group(2))
                    folw = value[found.end(1):]
                    value = prev + repl + folw
                else:
                    yield value
                    return
        else:
            yield value

    # --------------------------------------------------------------------------
    def _doSubEvals(self, expr, variables):
        for value in self._doSubVars(expr, variables):
            if type(value) is not str:
                yield value
            else:
                global keepRunning
                while keepRunning:
                    found = re.match(self._res.eval_exp, value)
                    if found:
                        prev = value[:found.start(1)]
                        repl = str(eval(found.group(2)))
                        folw = value[found.end(1):]
                        yield prev + repl + folw
                    else:
                        yield value
                        break

    # --------------------------------------------------------------------------
    def _doApplyCmds(self, expr, variables, cmds):
        if not cmds:
            for value in self._doSubEvals(expr, variables):
                yield value
        else:
            cmd_name, cmd_re, cmd_func = cmds[0]
            for value in self._doApplyCmds(expr, variables, cmds[1:]):
                if type(value) is not str:
                    yield value
                else:
                    found = re.match(cmd_re, value)
                    if found:
                        prev = value[:found.start(1)]
                        repl = cmd_func(found)
                        folw = value[found.end(1):]
                        if type(repl) is not list:
                            repl = [repl]
                        for item in repl:
                            if prev or folw:
                                yield prev + str(item) + folw
                            else:
                                yield item
                    else:
                        yield value

    # --------------------------------------------------------------------------
    def _doSubstitute(self, expr, variables):
        for value in self._doApplyCmds(expr, variables, self._res.commands):
            yield value

    # --------------------------------------------------------------------------
    def _substitute(self, exprs, variables):
        if type(exprs) is not list:
            exprs = [exprs]

        for expr in exprs:
            for value in self._doSubstitute(expr, variables):
                if type(value) is not str:
                    yield value
                else:
                    found = re.match(self._res.list_exp, value)
                    if found:
                        prev = value[:found.start(1)]
                        repl = variables.get(found.group(2), [])
                        folw = value[found.end(1):]
                        if type(repl) is not list:
                            repl = [repl]

                        for nested_value in self._substitute(
                            [prev+str(item)+folw for item in repl],
                            variables):
                            yield nested_value
                    else:
                        found = re.match(self._res.sbsc_exp, value)
                        if found:
                            prev = value[:found.start(1)]
                            repl = variables.get(found.group(2), [])
                            folw = value[found.end(1):]
                            indx = int(found.group(3))

                            if prev or folw:
                                value = prev + str(repl) + folw
                            else:
                                value = repl

                            repl = [e for e in self._substitute(value, variables)]

                            try:
                                yield repl[indx % len(repl)]
                            except ValueError:
                                yield value
                        else:
                            yield value

    # --------------------------------------------------------------------------
    def resolve(self, exprs, variables):
        return list(self._substitute(exprs, variables))

    # --------------------------------------------------------------------------
    def _doApplyAdjs(self, inst, expr, variables, adjs):
        if not adjs:
            yield expr
        else:
            adj_name, adj_re, adj_func = adjs[0]
            for value in self._doApplyAdjs(inst, expr, variables, adjs[1:]):
                if type(value) is not str:
                    yield value
                else:
                    found = re.match(adj_re, value)
                    if found:
                        prev = value[:found.start(1)]
                        repl = adj_func(inst, found)
                        folw = value[found.end(1):]
                        if type(repl) is not list:
                            repl = [repl]
                        for item in repl:
                            if prev and folw:
                                yield prev + str(item) + folw
                            else:
                                yield item
                    else:
                        yield value

    # --------------------------------------------------------------------------
    def adjustArgsOf(self, instance, arg, variables):
        for value in self._substitute(
            list(self._doApplyAdjs(
                instance, arg, variables, self._res.adjustments)),
            variables):
            yield value

    # --------------------------------------------------------------------------
    def pipelineConfigs(self):
        return self.full_config.get("pipelines", [])

    # --------------------------------------------------------------------------
    def cleanShutdownExpectations(self):
        expectations = self.full_config.get("expect", {})
        return dict(
            (identity, ExpectCleanShutdown(identity))
            for identity in expectations.get("clean_shutdown", []))

    # --------------------------------------------------------------------------
    def exitCodeExpectations(self):
        expectations = self.full_config.get("expect", {})
        return dict(
            (identity, ExpectExitCode(identity, int(value)))
            for identity, value in expectations.get("exit_code", {}).items())

# ------------------------------------------------------------------------------
class ProcessInstance(object):
    # --------------------------------------------------------------------------
    def __init__(self, parent, args):
        self._parent = parent
        self._args = [str(arg) for arg in args]
        self._identity = None
        for i in range(len(self._args) - 1):
            if self._args[i] == "--log-identity":
                self._identity = self._args[i + 1]
                break
        self._handle = subprocess.Popen(self._args)
        if self._handle:
            self._exit_code = None
        else:
            self._exit_code = -1
            self.onProcessExit()

    # --------------------------------------------------------------------------
    def identity(self):
        return self._identity

    # --------------------------------------------------------------------------
    def terminate(self):
        if self._exit_code is None:
            self._handle.send_signal(signal.SIGTERM)

    # --------------------------------------------------------------------------
    def kill(self):
        if self._exit_code is None:
            self._handle.send_signal(signal.SIGKILL)

    # --------------------------------------------------------------------------
    def exitCode(self):
        return self._exit_code

    # --------------------------------------------------------------------------
    def onProcessExit(self):
        if self._identity is not None:
            assert self._exit_code is not None
            self._parent.onProcessExit(self)

    # --------------------------------------------------------------------------
    def handleExit(self, pid):
        if self._handle.pid == pid:
            self._handle.wait()
            self._exit_code = self._handle.returncode
            self.onProcessExit()

    # --------------------------------------------------------------------------
    def isRunning(self):
        return self._handle is not None and not self.isFinished()

    # --------------------------------------------------------------------------
    def isFinished(self):
        if self._exit_code is None:
            self._exit_code = self._handle.poll()
            if self._exit_code is not None:
                self.onProcessExit()

        return self._exit_code is not None

# ------------------------------------------------------------------------------
class PipelineInstance(object):
    # --------------------------------------------------------------------------
    def __init__(self, parent, index):
        self._parent = parent
        self._index = index
        self._processes = [
            ProcessInstance(self, args)
            for args in parent.commandLineOf(self)]

    # --------------------------------------------------------------------------
    def instanceIndex(self):
        return self._index

    # --------------------------------------------------------------------------
    def terminate(self):
        for process in self._processes:
            process.terminate()
            break # only the first one

    # --------------------------------------------------------------------------
    def kill(self):
        for process in self._processes:
            process.kill()

    # --------------------------------------------------------------------------
    def onProcessExit(self, process):
        self._parent.onProcessExit(self._index, process)
        if self.isFinished():
            self._parent.onInstanceFinished(self)

    # --------------------------------------------------------------------------
    def handleExit(self, pid):
        for process in self._processes:
            process.handleExit(pid)

    # --------------------------------------------------------------------------
    def isRunning(self):
        return self._processes is not None and \
            all(p.isRunning() for p in self._processes)

    # --------------------------------------------------------------------------
    def isFinished(self):
        return self._processes is None or \
            any(p.isFinished() for p in self._processes)

# ------------------------------------------------------------------------------
class Pipeline(object):
    # --------------------------------------------------------------------------
    def __init__(self, options, parent, config):
        self._parent = parent
        self._config = config
        self._instance_counter = 0
        if config.get("instances") == "*":
            self._required_instances = None
        else:
            self._required_instances = int(config.get("instances", 1))
            assert self._required_instances > 0
        parallel = config.get("parallel", 1)
        if type(parallel) is bool:
            if self._required_instances is not None:
                parallel = self._required_instances
            else:
                parallel = 1
        self._parallel_instances = parallel
        assert self._parallel_instances > 0
        self._instances = [None] * self._parallel_instances
        self._termination_start = None

    # --------------------------------------------------------------------------
    def _prepareArgs(self, instance, args):
        variables = self._config.get("variables", {})
        for arg in args:
            for adjusted in self._parent.adjustArgsOf(instance, arg, variables):
                yield adjusted

    # --------------------------------------------------------------------------
    def commandLineOf(self, instance):
        commands = self._config["commands"]
        for args in commands:
            yield self._prepareArgs(instance, args)

    # --------------------------------------------------------------------------
    def identity(self):
        return self._config.get("identity")

    # --------------------------------------------------------------------------
    def keepRestarting(self):
        return self._required_instances is None

    # --------------------------------------------------------------------------
    def isRequiredBy(self, pipeline_identity):
        for identity in self._config.get("required_by", []):
            if pipeline_identity == identity:
                return True
        return False

    # --------------------------------------------------------------------------
    def needsNewInstance(self):
        if self.keepRestarting():
            return True
        if self._instance_counter < self._required_instances:
            return True
        return False

    # --------------------------------------------------------------------------
    def terminate(self):
        for instance in self._instances:
            if instance is not None:
                instance.terminate()

    # --------------------------------------------------------------------------
    def kill(self):
        for instance in self._instances:
            if instance is not None:
                instance.kill()

    # --------------------------------------------------------------------------
    def onProcessExit(self, instance_index, process):
        self._parent.onProcessExit(self, instance_index, process)

    # --------------------------------------------------------------------------
    def onInstanceFinished(self, index):
        if self.isFinished():
            self._parent.onPipelineFinished(self)

    # --------------------------------------------------------------------------
    def handleExit(self, pid):
        for instance in self._instances:
            if instance is not None:
                instance.handleExit(pid)

    # --------------------------------------------------------------------------
    def isRunning(self):
        if len(self._instances) == 0:
            return False
        for instance in self._instances:
            if instance is None or not instance.isRunning():
                return False
        return True

    # --------------------------------------------------------------------------
    def areInstancesFinished(self):
        for instance in self._instances:
            if instance is not None and not instance.isFinished():
                return False
        return True

    # --------------------------------------------------------------------------
    def isFinished(self):
        if not self.areInstancesFinished():
            return False

        if self.needsNewInstance():
            return False
        return True

    # --------------------------------------------------------------------------
    def isWaitingForOthers(self):
        return not self._parent.areRequirementsRunning(self)

    # --------------------------------------------------------------------------
    def shouldBeRunning(self):
        if self._parent.isRequiredByOthers(self):
            return True
        return self._required_instances is not None

    # --------------------------------------------------------------------------
    def manageInstance(self, instance, tracker):
        if instance is not None and instance.isFinished():
            instance = None

        if instance is None:
            if self.needsNewInstance() and not self.isWaitingForOthers():
                instance = PipelineInstance(self, self._instance_counter)
                self._instance_counter += 1

        return instance

    # --------------------------------------------------------------------------
    def manage(self, tracker):
        if self.shouldBeRunning():
            self._instances = \
                [self.manageInstance(i, tracker) for i in self._instances]
        else:
            if self._termination_start is None:
                self._termination_start = time.time()
                self.terminate()
            elif (time.time() - self._termination_start) > 10:
                self.kill()
                return False
        return not self.isFinished()

# ------------------------------------------------------------------------------
class PipelineComposition(object):
    # --------------------------------------------------------------------------
    def __init__(self, options, config):
        self._config = config
        self._tracker = None
        self._pipelines = [
            Pipeline(options, self, cfg)
            for cfg in config.pipelineConfigs()]

    # --------------------------------------------------------------------------
    def setTracker(self, tracker):
        self._tracker = tracker

    # --------------------------------------------------------------------------
    def config(self):
        return self._config

    # --------------------------------------------------------------------------
    def adjustArgsOf(self, instance, arg, variables):
        for value in self._config.adjustArgsOf(instance, arg, variables):
            yield value

    # --------------------------------------------------------------------------
    def areRequirementsRunning(self, tested_pipeline):
        for pipeline in self._pipelines:
            if pipeline.isRequiredBy(tested_pipeline.identity()):
                if not pipeline.isRunning():
                    return False

        return True

    # --------------------------------------------------------------------------
    def isRequiredByOthers(self, tested_pipeline):
        for pipeline in self._pipelines:
            if tested_pipeline.isRequiredBy(pipeline.identity()):
                return True

        return False

    # --------------------------------------------------------------------------
    def onProcessExit(self, pipeline, pipeline_instance, process):
        assert self._tracker is not None
        self._tracker.onProcessExit(pipeline, pipeline_instance, process)

    # --------------------------------------------------------------------------
    def onPipelineFinished(self, pipeline):
        assert self._tracker is not None
        self._tracker.onPipelineFinished(pipeline)

    # --------------------------------------------------------------------------
    def handleExit(self, pid):
        for pl in self._pipelines:
            pl.handleExit(pid)

    # --------------------------------------------------------------------------
    def manage(self, tracker):
        self._pipelines = [
            pipeline for pipeline in self._pipelines
            if pipeline.manage(tracker)]
        return len(self._pipelines) > 0

# ------------------------------------------------------------------------------
composition = None
# ------------------------------------------------------------------------------
class XmlLogProcessor(xml.sax.ContentHandler):
    # --------------------------------------------------------------------------
    def __init__(self, src_id, tracker):
        self._src_id = src_id
        self._clean_shutdown = False
        self._ctag = None
        self._carg = None
        self._info = None
        self._loggers = {}
        self._source_map = {}
        self._start_time = time.time()
        self._tracker = tracker
        self._options = self._tracker._options
        self._parser = xml.sax.make_parser()
        self._parser.setContentHandler(self)

    # --------------------------------------------------------------------------
    def startElement(self, tag, attr):
        time_ofs = self._start_time - self._tracker._start_time
        self._ctag = tag
        if tag == "log":
            self._tracker.beginLog(self._src_id, attr)
        elif tag == "m":
            self._info = {
                r: attr.get(k, None) for k, r in [
                    ("lvl", "level"),
                    ("src", "source"),
                    ("tag", "tag"),
                    ("iid", "instance"),
                    ("ts",  "timestamp")
                ]
            }
            self._info["args"] = {}
            self._source_map[attr["iid"]] = attr["src"]
        elif tag == "a":
            self._carg = attr["n"]
            iarg = {
                "value": "''",
                "min": attr.get("min"),
                "max": attr.get("max"),
                "type": attr["t"],
                "blob": attr.get("blob", False)
            }
            try: self._info["args"][self._carg]["values"].append(iarg)
            except KeyError:
                self._info["args"][self._carg] = {
                    "values": [iarg]
                }
        elif tag == "i":
            pass
        elif tag == "c":
            try: logger = self._loggers[attr["src"]]
            except KeyError:
                logger = self._loggers[attr["src"]] = {}
            try: inst = logger[attr["iid"]]
            except KeyError:
                inst = logger[attr["iid"]] = {}
            try: charts = inst["charts"]
            except KeyError:
                charts = inst["charts"] = {}
            try: series = charts[attr["ser"]]
            except KeyError:
                series = charts[attr["ser"]] = []
            series.append((time_ofs+float(attr["ts"]), float(attr["v"])))
        elif tag == "d":
            try: logger = self._loggers[attr["src"]]
            except KeyError:
                logger = self._loggers[attr["src"]] = {}
            try: inst = logger[attr["iid"]]
            except KeyError:
                inst = logger[attr["iid"]] = {}
            inst["display_name"] = attr["dn"]
            inst["description"] = attr["desc"]

    # --------------------------------------------------------------------------
    def endElement(self, tag):
        if tag == "log":
            self._clean_shutdown = True
        elif tag == "m":
            self._tracker.addMessage(self._src_id, self._info)
            self._info = None

    # --------------------------------------------------------------------------
    def characters(self, content):
        if self._info:
            if self._ctag == "f":
                self._info["format"] = content
            elif self._ctag == "a":
                self._info["args"][self._carg]["values"][-1]["value"] = content

    # --------------------------------------------------------------------------
    def processLine(self, line):
        self._parser.feed(line)

    # --------------------------------------------------------------------------
    def processDisconnect(self):
        self._tracker.addLoggerInfos(self._src_id, self._loggers)
        self._tracker.finishLog(self._src_id, self._clean_shutdown)

# ------------------------------------------------------------------------------
class XmlLogClientHandler(xml.sax.ContentHandler):

    # --------------------------------------------------------------------------
    def __init__(self, processor, connection, selector):
        self._processor = processor
        self._connection = connection
        self._selector = selector
        self._buffer = bytes()

    # --------------------------------------------------------------------------
    def __del__(self):
        self._connection.close()

    # --------------------------------------------------------------------------
    def disconnect(self):
        self._processor.processLine(self._buffer)
        self._processor.processDisconnect()
        self._selector.unregister(self._connection)

    # --------------------------------------------------------------------------
    def handleRead(self):
        try:
            data = self._connection.recv(4096)
            if data:
                sep = bytes('\n','utf-8')
                self._buffer += data
                lines = self._buffer.split(sep)
                done = 0
                for line in lines[:-1]:
                    try:
                        line = line.decode('utf-8')
                        self._processor.processLine(line)
                        done += 1
                    except UnicodeDecodeError:
                        logging.error("failed to decode: '%s'" % line)
                        break
                    except:
                        logging.error("failed parse XML: '%s'" % line)
                        break
                self._buffer = sep.join(lines[done:])
            else:
                self.disconnect()
        except socket.error:
            self.disconnect()
# ------------------------------------------------------------------------------
#  Tracker
# ------------------------------------------------------------------------------
class ProcessLogTracker(object):
    # --------------------------------------------------------------------------
    def __init__(self, options, composition):
        self._options = options
        self._composition = composition
        self._start_time = time.time()
        self._source_id = 0
        self._loggers = {}
        self._identities = {}

        self._expect_clean_shutdown =\
            composition.config().cleanShutdownExpectations()
        self._expect_exit_code =\
            composition.config().exitCodeExpectations()

    # --------------------------------------------------------------------------
    def beginLog(self, src_id, info):
        if "identity" in  info:
            self._identities[src_id] = info["identity"]
            print("beginLog", self._identities)

    # --------------------------------------------------------------------------
    def finishLog(self, src_id, clean_shutdown):
        try:
            identity = self._identities[src_id]
            self._expect_clean_shutdown[identity].update(clean_shutdown)
        except KeyError: pass

        try: del self._identities[src_id]
        except KeyError: pass

    # --------------------------------------------------------------------------
    def addMessage(self, src_id, info):
        pass

    # --------------------------------------------------------------------------
    def addLoggerInfos(self, src_id, infos):
        self._loggers[src_id] = infos

    # --------------------------------------------------------------------------
    def makeProcessor(self):
        self._source_id += 1
        return XmlLogProcessor(self._source_id, self)

    # --------------------------------------------------------------------------
    def onProcessExit(self, pipeline, pipeline_index, process):
        try:
            self._expect_exit_code[process.identity()]\
                .update(process.identity(), process.exitCode())
        except KeyError: pass

    # --------------------------------------------------------------------------
    def onPipelineFinished(self, pipeline):
        # TODO
        print("pipeline", pipeline.identity())

    # --------------------------------------------------------------------------
    def allExpectations(self):
        for exp in self._expect_clean_shutdown.values():
            yield exp
        for exp in self._expect_exit_code.values():
            yield exp

    # --------------------------------------------------------------------------
    def result(self):
        success = True
        for expectation in self.allExpectations():
            success = expectation.check() and success
        return 0 if success else 1

# ------------------------------------------------------------------------------
class LocalLogSocket(socket.socket):
    # --------------------------------------------------------------------------
    def __init__(self, socket_path):
        self._socket_path = socket_path
        try: os.unlink(self._socket_path)
        except OSError as os_error:
            if os_error.errno != errno.ENOENT:
                raise
        socket.socket.__init__(self, socket.AF_UNIX, socket.SOCK_STREAM)
        self.bind(self._socket_path)
        self.listen(50)
        self.setblocking(False)

    # --------------------------------------------------------------------------
    def __del__(self):
        try:
            self.close()
            os.unlink(self._socket_path)
        except: pass

# ------------------------------------------------------------------------------
#  Main loop
# ------------------------------------------------------------------------------
def manageProcesses(options, composition, tracker):
    global keepRunning
    with selectors.DefaultSelector() as selector:
        log_sock = LocalLogSocket(options.logSocketPath())
        selector.register(
            log_sock,
            selectors.EVENT_READ,
            data = tracker
        )

        while keepRunning:
            if not composition.manage(tracker):
                break
            events = selector.select(timeout=1.0)
            for key, mask in events:
                if type(key.data) is ProcessLogTracker:
                    connection, addr = log_sock.accept()
                    connection.setblocking(False)
                    selector.register(
                        connection,
                        selectors.EVENT_READ,
                        data = XmlLogClientHandler(
                            key.data.makeProcessor(),
                            connection,
                            selector
                        )
                    )
                elif type(key.data) is XmlLogClientHandler:
                    if mask & selectors.EVENT_READ:
                        key.data.handleRead()

    return tracker.result()

# ------------------------------------------------------------------------------
#  Signal handling
# ------------------------------------------------------------------------------
def handleInterrupt(sig, frame):
    global keepRunning
    keepRunning = False
# ------------------------------------------------------------------------------
def handleChildExit(signum, frame):
    global composition

    try:
        pid, signum = os.waitpid(-1, os.WNOHANG)
        if composition:
            composition.handleExit(pid)
    except OSError:
        pass

# ------------------------------------------------------------------------------
#  Initialization and startup
# ------------------------------------------------------------------------------
def main():
    global composition
    signal.signal(signal.SIGINT, handleInterrupt)
    signal.signal(signal.SIGTERM, handleInterrupt)
    signal.signal(signal.SIGCHLD, handleChildExit)
    argparser = getArgumentParser()
    options = argparser.parseArgs()
    composition = PipelineComposition(options, PipelineConfig(options))
    tracker = ProcessLogTracker(options, composition)
    composition.setTracker(tracker)

    if not options.dry_run:
        return manageProcesses(options, composition, tracker)
    return 0

# ------------------------------------------------------------------------------
if __name__ == "__main__": sys.exit(main())

