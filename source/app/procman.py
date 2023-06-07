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

import xml.etree.ElementTree as etree

try:
    import selectors
except ImportError:
    import selectors2 as selectors

# ------------------------------------------------------------------------------
#  Utilities
# ------------------------------------------------------------------------------
def formatDuration(s):
    if s >= 60480000:
        return "%dw" % (int(s) / 604800)
    if s >= 604800:
        return "%2dw %2dd" % (int(s) / 604800, (int(s) % 604800) / 86400)
    if s >= 86400:
        return "%2dd %2dh" % (int(s) / 86400, (int(s) % 86400) / 3600)
    if s >= 3600:
        return "%2dh %02dm" % (int(s) / 3600, (int(s) % 3600) / 60)
    if s >= 60:
        return "%2dm %02ds" % (int(s) / 60, int(s) % 60)
    if s >= 15:
        return "%3ds" % int(round(s))
    if s >= 0.01:
        return "%4dms" % int(s*10**3)
    if s <= 0.0:
        return "0s"
    return "%dμs" % int(s*10**6)

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

        self.add_argument(
            "--debug", "-D",
            action="store_true",
            default=False,
            help="""Starts process manager in debug mode."""
        )

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
                Specifies new values for the configuration variables.
                Variables from loaded configuration files are always overridden,
                by values specified on the command-line.
            """
        )

        def ident_positive_int(arg):
            sep = '='
            tmp = arg.split(sep)
            tmp = (tmp[0], int(sep.join(tmp[1:])))
            assert tmp[1] > 0
            return tmp

        self.add_argument(
            "--instances", "-I",
            dest="instance_counts",
            metavar="identifier=integer",
            type=ident_positive_int,
            action="append",
            default=[],
            help="""
                Specifies new values for the pipeline instance counts.
                Counts from loaded configuration files are always overridden,
                by values specified on the command-line.
            """
        )

        self.add_argument(
            "--parallel", "-P",
            dest="parallel_counts",
            metavar="identifier=integer",
            type=ident_positive_int,
            action="append",
            default=[],
            help="""
                Specifies new values for the parallel pipeline instance counts.
                Counts from loaded configuration files are always overridden,
                by values specified on the command-line.
            """
        )

        wrappers = self.add_mutually_exclusive_group()

        wrappers.add_argument(
            "--cachegrind",
            action="store_true",
            default=False,
            help="""Runs EAGine applications in valgrind --cachegrind"""
        )

        wrappers.add_argument(
            "--callgrind",
            action="store_true",
            default=False,
            help="""Runs EAGine applications in valgrind --callgrind"""
        )

        wrappers.add_argument(
            "--memcheck",
            action="store_true",
            default=False,
            help="""Runs EAGine applications in valgrind --memcheck"""
        )

        wrappers.add_argument(
            "--massif",
            action="store_true",
            default=False,
            help="""Runs EAGine applications in valgrind --massif"""
        )

        wrappers.add_argument(
            "--helgrind",
            action="store_true",
            default=False,
            help="""Runs EAGine applications in valgrind --helgrind"""
        )

        self.add_argument(
            "--dry-run",
            action="store_true",
            default=False,
            help="""Does not actually start anything just prints selected information."""
        )

        self.add_argument(
            "--print-config", "-C",
            action="store_true",
            default=False,
            help="""Prints the fully loaded and merged process configuration."""
        )

        self.add_argument(
            "--forward-local-socket", "-l",
            dest='local_socket',
            action="store",
            default=False
        )

        self.add_argument(
            "--forward-network-socket", "-n",
            dest='network_socket',
            action="store",
            default=False
        )

    # --------------------------------------------------------------------------
    def processParsedOptions(self, options):
        if options.local_socket == "-":
            options.local_socket = "/tmp/eagine-xmllog"
        if options.network_socket == "-":
            options.network_socket = "localhost:34917"
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
                lambda inst, mtch: inst.index()
            ),
            ("uid",
                re.compile(".*(\$<uid>).*"),
                lambda inst, mtch: self._resolveCmdUid(None)
            )
        ]

        self.variable = re.compile(".*(\${([A-Za-z][A-Za-z_0-9]*)}).*")
        self.list_exp = re.compile(".*(\$\[([A-Za-z][A-Za-z_0-9]*)\.\.\.\]).*")
        self.sbsc_exp = re.compile(".*(\$\[([A-Za-z][A-Za-z_0-9]*)\[([0-9]+)\]\]).*")
        self.eval_exp = re.compile(".*(\$\(([0-9+*/%-]*)\)).*")

    # --------------------------------------------------------------------------
    def _addCmdWrappers(self, args):
        if self._options.memcheck:
            return ["valgrind", "--tool=memcheck"] + args
        elif self._options.callgrind:
            return ["valgrind", "--tool=callgrind"] + args
        elif self._options.cachegrind:
            return ["valgrind", "--tool=cachegrind"] + args
        elif self._options.helgrind:
            return ["valgrind", "--tool=helgrind"] + args
        elif self._options.massif:
            return ["valgrind", "--tool=massif"] + args
        return args

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
        return ["--session-identity", expr]

    # --------------------------------------------------------------------------
    def _resolveCmdEAGiApp(self, name):
        file_dir = os.path.dirname(__file__)
        def _get_path_from(filename):
            bdf_path = os.path.join(file_dir, os.pardir, os.pardir, filename)
            try:
                with open(bdf_path, "r") as bdf:
                    return os.path.realpath(os.path.join(
                            bdf.readline().strip(), os.pardir, os.pardir))
            except: pass

        def _get_build_dir():
            return _get_path_from("BINARY_DIR")

        def _get_install_prefix():
            return _get_path_from("INSTALL_PREFIX")

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
            yield _scanopts(_get_install_prefix())
            yield self._resolveCmdWildcard(name)

        for found in _search():
            if found:
                return self._addCmdWrappers([
                    found,
                    "--use-asio-log",
                    self._options.logSocketPath()])

        raise RuntimeError("Failed to find eagine application '%s'" % name)

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
#  Log message info
# ------------------------------------------------------------------------------
class LogMessageInfo(object):
    # --------------------------------------------------------------------------
    def __init__(self, message_info, state_info):
        self._et = etree.Element("root")
        self._et.append(self._maketree(message_info, "log"))
        self._et.append(self._maketree(state_info, "state"))

    # --------------------------------------------------------------------------
    def find(self, xpath):
        return self._et.findall(xpath)

    # --------------------------------------------------------------------------
    def has(self, xpath):
        return len(self.find(xpath)) > 0

    # --------------------------------------------------------------------------
    def _item2etree(self, name, value, node):
        if type(value) is dict:
            self._dict2etree(value, etree.SubElement(node, name))
        elif type(value) is list:
            for elem in value:
                self._item2etree(name, elem, node)
            pass
        else:
            node.set(name, str(value))

    # --------------------------------------------------------------------------
    def _dict2etree(self, source, node):
        for name, value in source.items():
            self._item2etree(name, value, node)
        return node

    # --------------------------------------------------------------------------
    def _maketree(self, source, root_name):
        root = etree.Element(root_name)
        return self._dict2etree(source, root)

# ------------------------------------------------------------------------------
#  Expectations
# ------------------------------------------------------------------------------
def getCompareFunc(operator):
    if operator == "=":
        return lambda l, r: l == r
    if operator == "!=":
        return lambda l, r: l != r
    if operator == "<=":
        return lambda l, r: l <= r
    if operator == ">=":
        return lambda l, r: l >= r
    if operator == "<":
        return lambda l, r: l < r
    if operator == ">":
        return lambda l, r: l > r

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
    def check(self, tracker):
        if self._satisfied:
            return True

        tracker.log().error(
            "unexpected failed shutdown of '%s'",
            self._identity)
        return False

# ------------------------------------------------------------------------------
class ExpectExitCode(object):
    # --------------------------------------------------------------------------
    def __init__(self, identity, value):
        assert type(value) is int
        self._expected_value = value
        self._actual_values = []
        self._identity = identity

    # --------------------------------------------------------------------------
    def update(self, instance, value):
        self._actual_values.append(value)

    # --------------------------------------------------------------------------
    def check(self, tracker):
        actual = set(self._actual_values)
        if all(v == self._expected_value for v in actual):
            return True

        if len(actual) == 0:
            tracker.log().error(
                "did not receive exit notification of '%s'",
                self._identity)

        for value in actual:
            tracker.log().error(
                "unexpected exit code %d of '%s', expected: %d",
                value,
                self._identity,
                self._expected_value)

        return False

# ------------------------------------------------------------------------------
class ExpectRunTime(object):
    # --------------------------------------------------------------------------
    @staticmethod
    def factory():
        # ----------------------------------------------------------------------
        class _Factory(object):
            # ------------------------------------------------------------------
            def __init__(self):
                self._re = re.compile("^(=|!=|<|>|<=|>=)([0-9]+)$");

            # ------------------------------------------------------------------
            def __call__(self, identity, data):
                parsed = re.match(self._re, data)
                if parsed:
                    return ExpectRunTime(
                        identity,
                        parsed.group(1),
                        float(parsed.group(2)))
                return None

        return _Factory()

    # --------------------------------------------------------------------------
    def __init__(self, identity, operator, value):
        self._identity = identity
        self._operator = operator
        self._compare = getCompareFunc(operator)
        self._expected_value = value
        self._actual_values = []

    # --------------------------------------------------------------------------
    def update(self, value):
        self._actual_values.append(value)

    # --------------------------------------------------------------------------
    def check(self, tracker):
        result = True
        for value in self._actual_values:
            if not self._compare(value, self._expected_value):
                tracker.log().error(
                    "run time %s of '%s' is expected to be %s %s",
                    formatDuration(value),
                    self._identity,
                    self._operator,
                    formatDuration(self._expected_value))
                result = False
        return result

# ------------------------------------------------------------------------------
class ExpectXPathCount(object):
    # --------------------------------------------------------------------------
    @staticmethod
    def factory():
        # ----------------------------------------------------------------------
        class _Factory(object):
            # ------------------------------------------------------------------
            def __init__(self):
                self._re = re.compile("^count\((.*)\)(=|!=|<|>|<=|>=)([0-9]+)$")

            # ------------------------------------------------------------------
            def __call__(self, identity, data):
                parsed = re.match(self._re, data)
                if parsed:
                    return ExpectXPathCount(
                        identity,
                        found.group(1),
                        found.group(2),
                        int(found.group(3)))

        return _Factory()

    # --------------------------------------------------------------------------
    def __init__(self, identity, xpath, operator, value):
        self._identity = identity
        self._xpath = xpath
        self._operator = operator
        self._compare = getCompareFunc(operator)
        self._value = value
        self._count = 0

    # --------------------------------------------------------------------------
    def update(self, message_info):
        if message_info.has(self._xpath):
            self._count += 1

    # --------------------------------------------------------------------------
    def check(self, tracker):
        if self._compare(self._count, self._value):
            return True

        tracker.log().error(
            "count %d of '%s' from '%s' is expected to be %s %d",
            self._count,
            self._xpath,
            self._identity,
            self._operator,
            self._value
        )

        return False

# ------------------------------------------------------------------------------
class ExpectXPathAggregate(object):
    # --------------------------------------------------------------------------
    @staticmethod
    def factory():
        # ----------------------------------------------------------------------
        class _Factory(object):
            # ------------------------------------------------------------------
            def __init__(self):
                self._re = re.compile(
                    "^(sum|min|max|avg|lean)\((.*)/@(\w+)\)(=|!=|<|>|<=|>=)([0-9]+(\.[0-9]+)?)$")

            # ------------------------------------------------------------------
            def __call__(self, identity, data):
                parsed = re.match(self._re, data)
                if parsed:
                    return ExpectXPathAggregate(
                        identity,
                        parsed.group(1),
                        parsed.group(2),
                        parsed.group(3),
                        parsed.group(4),
                        float(parsed.group(5)))

        return _Factory()

    # --------------------------------------------------------------------------
    def __init__(self, identity, func_name, xpath, attrib, operator, value):
        self._identity = identity
        self._func_name = func_name
        self._functions = self._get_functions(func_name)
        self._xpath = xpath
        self._attrib = attrib
        self.operator = operator
        self._compare = getCompareFunc(operator)
        self._value = value
        self._aggregate = None

    # --------------------------------------------------------------------------
    def _get_functions(self, name):
        if name == "sum":
            return (
                lambda s, n: n if s is None else s + n,
                lambda s: s)
        if name == "min":
            return (
                lambda s, n: n if s is None else min(s, n),
                lambda s: s)
        if name == "max":
            return (
                lambda s, n: n if s is None else max(s, n),
                lambda s: s)
        if name == "avg":
            return (
                lambda s, n: (n, 1) if s is None else (s[0]+n, s[1]+1),
                lambda s: None if s is None else s[0] / s[1])
        if name == "lean":
            def _lean_step(s, n):
                if s is None:
                    return (n, n, n, 1)
                return (min(s[0], n), s[1]+n, max(s[2], n), s[3]+1)
            def _lean_calc(s):
                if s is None:
                    return None
                if s[0] >= s[2]:
                    return 0.0
                return (((s[1] / s[3] - s[0])/(s[2] - s[0])) - 0.5) * 2.0
            return (_lean_step, _lean_calc)

    # --------------------------------------------------------------------------
    def update(self, message_info):
        for node in message_info.find(self._xpath):
            attr_val = float(node.attrib[self._attrib])
            self._aggregate = self._functions[0](self._aggregate, attr_val)

    # --------------------------------------------------------------------------
    def check(self, tracker):
        final_val = self._functions[1](self._aggregate)
        if final_val is None or self._compare(final_val, self._value):
            return True

        tracker.log().error(
            "%s %f of '%s' from '%s' is expected to be %s %f",
            self._func_name,
            final_val,
            self._identity,
            self._xpath,
            self.operator,
            self._value)

        return False

# ------------------------------------------------------------------------------
#  Configuration
# ------------------------------------------------------------------------------
class PipelineConfig(object):
    # --------------------------------------------------------------------------
    def __init__(self, options):
        self._log = logging.getLogger("eagiproc.config")
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
                    self._log.error("reading '%s': %s" % (config_path, io_error))
                except ValueError as value_error:
                    self._log.error("parsing '%s': %s" % (config_path, io_error))

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

            for identity, inst_count in options.instance_counts:
                if pipeline.get("identity") == identity:
                    pipeline["instances"] = inst_count

            for identity, parl_count in options.parallel_counts:
                if pipeline.get("identity") == identity:
                    pipeline["parallel"] = parl_count

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

    # --------------------------------------------------------------------------
    def runTimeExpectations(self):
        expectations = self.full_config.get("expect", {}).get("run_time", {})
        factory = ExpectRunTime.factory()

        result = {}
        def _addToResult(identity, exp):
            if exp is not None:
                try:
                    dst = result[identity]
                except KeyError:
                    dst = result[identity] = []
                dst.append(exp)

        for identity, definitions in expectations.items():
            if type(definitions) is not list:
                definitions = [definitions]
            for data in definitions:
                _addToResult(identity, factory(identity, data))

        return result

    # --------------------------------------------------------------------------
    def logMessageInfoExpectations(self):
        expectations = self.full_config.get("expect", {}).get("xpath", {})
        factories = [
            ExpectXPathCount.factory(),
            ExpectXPathAggregate.factory()]

        result = {}
        def _addToResult(identity, exp):
            if exp is not None:
                try:
                    dst = result[identity]
                except KeyError:
                    dst = result[identity] = []
                dst.append(exp)
                return True
            return False

        for identity, definitions in expectations.items():
            if type(definitions) is not list:
                definitions = [definitions]
            for xpath in definitions:
                for factory in factories:
                    if _addToResult(identity, factory(identity, xpath)):
                        break

        return result

# ------------------------------------------------------------------------------
class ProcessInstance(object):
    # --------------------------------------------------------------------------
    def __init__(self, parent, args):
        self._parent = parent
        self._identity = None
        self._number = None
        self._progress = None

        if args is None:
            raise RuntimeError("Invalid argument list")
        args = [arg for arg in args]
        if not all(arg is not None for arg in args):
            raise RuntimeError("None value is arguments")
        self._args = [str(arg) for arg in args]
        if len(self._args) == 0:
            raise RuntimeError("Empty argument list")

        self._active_states = set()
        self._current_states = {}
        for i in range(len(self._args) - 1):
            if self._args[i] == "--session-identity":
                self._identity = self._args[i + 1]
            elif self._args[i] == "--log-identity":
                self._number = int(self._args[i + 1])
        if not self._args or self._args[0] is None or self._args[0] == "None":
            self.tracker().log().error(
                "failed to find executable in pipeline '%s'",
                parent.identity())
        self._handle = subprocess.Popen(self._args)
        self._start_time = time.time()
        if self._handle:
            self._exit_code = None
            self._progress = parent.makeProcessProgress(self)

    # --------------------------------------------------------------------------
    def __del__(self):
        self._exit_code = -1
        self.onProcessExit()

    # --------------------------------------------------------------------------
    def tracker(self):
        return self._parent.tracker()

    # --------------------------------------------------------------------------
    def identity(self):
        return self._identity

    # --------------------------------------------------------------------------
    def number(self):
        return self._number

    # --------------------------------------------------------------------------
    def states(self):
        return self._current_states.keys()

    # --------------------------------------------------------------------------
    def addActiveState(self, process_number, state):
        self._active_states.add(state)

    # --------------------------------------------------------------------------
    def handleStateBegin(self, process_number, state, begin_time):
        if self._number == process_number:
            state_info = {}
            state_info["begin_time"] = begin_time
            if state in self._active_states:
                state_info["is_active"] = True
            self.onStateBegin(state, state_info)
            self._current_states[state] = state_info

    # --------------------------------------------------------------------------
    def handleStateEnd(self, process_number, state, end_time):
        if self._number == process_number:
            try:
                state_info = self._current_states[state]
                state_info["end_time"] = end_time
                del self._current_states[state]
                self.onStateEnd(state, state_info)
            except KeyError: pass

    # --------------------------------------------------------------------------
    def updateProgress(self, process_number, progress_info):
        if self._number == process_number:
            self.onProgressChange(progress_info)

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
    def onStateBegin(self, state, state_info):
        if self._identity is not None:
            self._parent.onProcessStateBegin(self, state, state_info)

    # --------------------------------------------------------------------------
    def onStateEnd(self, state, state_info):
        if self._identity is not None:
            self._parent.onProcessStateEnd(self, state, state_info)

    # --------------------------------------------------------------------------
    def onProgressChange(self, progress_info):
        if self._identity is not None:
            self._parent.onProcessProgressChange(self, progress_info)
            if self._progress is not None:
                self._progress.update(progress_info)

    # --------------------------------------------------------------------------
    def onProcessExit(self):
        if self._identity is not None:
            assert self._exit_code is not None
            if self._progress is not None:
                self._progress.onProcessExit(self)
            self._parent.onProcessExit(self)
            self._identity = None

    # --------------------------------------------------------------------------
    def handleExit(self, pid):
        if self._handle.pid == pid:
            self._handle.wait()
            self._exit_code = self._handle.returncode
            self.onProcessExit()

    # --------------------------------------------------------------------------
    def isRunning(self):
        if self._handle is not None and not self.isFinished():
            if time.time() - self._start_time > 0.5: # [s]
                return True
        return False

    # --------------------------------------------------------------------------
    def isActive(self):
        if self.isRunning():
            if len(self._active_states) == 0:
                return True
            for state in self._current_states:
                if state in self._active_states:
                    return True
        return False

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
        self._processes = None
        self._progress = parent.makeInstanceProgress(self)
        self._processes = [
            ProcessInstance(self, args)
            for args in parent.commandLineOf(self)]

    # --------------------------------------------------------------------------
    def tracker(self):
        return self._parent.tracker()

    # --------------------------------------------------------------------------
    def pipeline(self):
        return self._parent

    # --------------------------------------------------------------------------
    def identity(self):
        return self._parent.identity()

    # --------------------------------------------------------------------------
    def index(self):
        return self._index

    # --------------------------------------------------------------------------
    def makeProcessProgress(self, process):
        return self._progress.processProgress(self._progress, process)

    # --------------------------------------------------------------------------
    def addProcessActiveState(self, process_number, state):
        for process in self._processes:
            if process is not None:
                process.addActiveState(process_number, state)

    # --------------------------------------------------------------------------
    def handleProcessStateBegin(self, process_number, state, begin_time):
        for process in self._processes:
            if process is not None:
                process.handleStateBegin(process_number, state, begin_time)

    # --------------------------------------------------------------------------
    def handleProcessStateEnd(self, process_number, state, end_time):
        for process in self._processes:
            if process is not None:
                process.handleStateEnd(process_number, state, end_time)

    # --------------------------------------------------------------------------
    def updateProcessProgress(self, process_number, info):
        for process in self._processes:
            if process is not None:
                process.updateProgress(process_number, info)

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
    def onProcessStateBegin(self, process, state, state_info):
        self._parent.onProcessStateBegin(self._index, process, state, state_info)

    # --------------------------------------------------------------------------
    def onProcessStateEnd(self, process, state, state_info):
        self._parent.onProcessStateEnd(self._index, process, state, state_info)

    # --------------------------------------------------------------------------
    def onProcessProgressChange(self, process, progress_info):
        self._parent.onProcessProgressChange(self._index, process, progress_info)

    # --------------------------------------------------------------------------
    def onProcessExit(self, process):
        self._parent.onProcessExit(self._index, process)
        self._progress.onProcessExit(self)
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
    def isActive(self):
        return self._processes is not None and \
            all(p.isActive() for p in self._processes)

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
        self._finished_instances = 0
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
        self._progress = parent.makePipelineProgress(self)

    # --------------------------------------------------------------------------
    def _prepareArgs(self, instance, args):
        variables = self._config.get("variables", {})
        for arg in args:
            for adjusted in self._parent.adjustArgsOf(instance, arg, variables):
                yield adjusted
        yield "--log-identity"
        yield str(self.newProcessNumber())

    # --------------------------------------------------------------------------
    def commandLineOf(self, instance):
        commands = self._config["commands"]
        for args in commands:
            yield self._prepareArgs(instance, args)

    # --------------------------------------------------------------------------
    def tracker(self):
        return self._parent.tracker()

    # --------------------------------------------------------------------------
    def makeInstanceProgress(self, instance):
        return self._progress.instanceProgress(self._progress, instance)

    # --------------------------------------------------------------------------
    def newProcessNumber(self):
        return self._parent.newProcessNumber()

    # --------------------------------------------------------------------------
    def requiredInstances(self):
        return self._required_instances

    # --------------------------------------------------------------------------
    def startedInstances(self):
        return self._instance_counter

    # --------------------------------------------------------------------------
    def finishedInstances(self):
        return self._finished_instances

    # --------------------------------------------------------------------------
    def addProcessActiveState(self, process_number, state):
        for instance in self._instances:
            if instance is not None:
                instance.addProcessActiveState(process_number, state)

    # --------------------------------------------------------------------------
    def handleProcessStateBegin(self, process_number, state, begin_time):
        for instance in self._instances:
            if instance is not None:
                instance.handleProcessStateBegin(
                    process_number, state, begin_time)

    # --------------------------------------------------------------------------
    def handleProcessStateEnd(self, process_number, state, end_time):
        for instance in self._instances:
            if instance is not None:
                instance.handleProcessStateEnd(process_number, state, end_time)

    # --------------------------------------------------------------------------
    def updateProcessProgress(self, process_number, info):
        for instance in self._instances:
            if instance is not None:
                instance.updateProcessProgress(process_number, info)

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
    def onProcessStateBegin(self, instance_index, process, state, state_info):
        self._parent.onProcessStateBegin(
            self, instance_index, process, state, state_info)

    # --------------------------------------------------------------------------
    def onProcessStateEnd(self, instance_index, process, state, state_info):
        self._parent.onProcessStateEnd(
            self, instance_index, process, state, state_info)

    # --------------------------------------------------------------------------
    def onProcessProgressChange(self, instance_index, process, progress_info):
        self._parent.onProcessProgressChange(
            self, instance_index, process, progress_info)

    # --------------------------------------------------------------------------
    def onProcessExit(self, instance_index, process):
        self._parent.onProcessExit(self, instance_index, process)

    # --------------------------------------------------------------------------
    def onInstanceFinished(self, instance):
        self._finished_instances += 1
        self._parent.onPipelineInstanceFinished(instance)
        self._progress.instanceFinished(self)
        if self.isFinished():
            self._parent.onPipelineFinished(self)
            self._progress.finished(self)

    # --------------------------------------------------------------------------
    def handleExit(self, pid):
        for instance in self._instances:
            if instance is not None:
                instance.handleExit(pid)

    # --------------------------------------------------------------------------
    def isActive(self):
        if len(self._instances) == 0:
            return False
        for instance in self._instances:
            if instance is None or not instance.isActive():
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
        return not self._parent.areRequirementsActive(self)

    # --------------------------------------------------------------------------
    def shouldBeRunning(self):
        global keepRunning
        if not keepRunning:
            return False
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
                self._progress.instanceStarted(self)

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
            elif (time.time() - self._termination_start) > 30:
                self.kill()
                return False
        return not self.isFinished()

# ------------------------------------------------------------------------------
class PipelineComposition(object):
    # --------------------------------------------------------------------------
    def __init__(self, options, config):
        self._config = config
        self._options = options
        self._tracker = None
        self._process_number = 0

    # --------------------------------------------------------------------------
    def tracker(self):
        return self._tracker

    # --------------------------------------------------------------------------
    def setTracker(self, tracker):
        self._tracker = tracker
        self._pipelines = [
            Pipeline(self._options, self, cfg)
            for cfg in self._config.pipelineConfigs()]

    # --------------------------------------------------------------------------
    def makePipelineProgress(self, pipeline):
        return self._tracker.makePipelineProgress(pipeline)

    # --------------------------------------------------------------------------
    def newProcessNumber(self):
        self._process_number += 1
        return self._process_number

    # --------------------------------------------------------------------------
    def addProcessActiveState(self, process_number, state):
        for pipeline in self._pipelines:
            pipeline.addProcessActiveState(process_number, state)

    # --------------------------------------------------------------------------
    def handleProcessStateBegin(self, process_number, state, begin_time):
        for pipeline in self._pipelines:
            pipeline.handleProcessStateBegin(
                process_number, state, begin_time)

    # --------------------------------------------------------------------------
    def handleProcessStateEnd(self, process_number, state, end_time):
        for pipeline in self._pipelines:
            pipeline.handleProcessStateEnd(process_number, state, end_time)

    # --------------------------------------------------------------------------
    def updateProcessProgress(self, process_number, info):
        for pipeline in self._pipelines:
            pipeline.updateProcessProgress(process_number, info)

    # --------------------------------------------------------------------------
    def config(self):
        return self._config

    # --------------------------------------------------------------------------
    def adjustArgsOf(self, instance, arg, variables):
        for value in self._config.adjustArgsOf(instance, arg, variables):
            yield value

    # --------------------------------------------------------------------------
    def areRequirementsActive(self, tested_pipeline):
        for pipeline in self._pipelines:
            if pipeline.isRequiredBy(tested_pipeline.identity()):
                if not pipeline.isActive():
                    return False

        return True

    # --------------------------------------------------------------------------
    def isRequiredByOthers(self, tested_pipeline):
        for pipeline in self._pipelines:
            if tested_pipeline.isRequiredBy(pipeline.identity()):
                return True

        return False

    # --------------------------------------------------------------------------
    def onProcessStateBegin(self, pipeline, instance_index, process, state, state_info):
        self._tracker.onProcessStateBegin(
            pipeline, instance_index, process, state, state_info)

    # --------------------------------------------------------------------------
    def onProcessStateEnd(self, pipeline, instance_index, process, state, state_info):
        self._tracker.onProcessStateEnd(
            pipeline, instance_index, process, state, state_info)

    # --------------------------------------------------------------------------
    def onProcessProgressChange(self, pipeline, instance_index, process, progress_info):
        self._tracker.onProcessProgressChange(
            pipeline, instance_index, process, progress_info)

    # --------------------------------------------------------------------------
    def onProcessExit(self, pipeline, pipeline_instance, process):
        assert self._tracker is not None
        self._tracker.onProcessExit(pipeline, pipeline_instance, process)

    # --------------------------------------------------------------------------
    def onPipelineInstanceFinished(self, instance):
        assert self._tracker is not None
        self._tracker.onPipelineInstanceFinished(instance)

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
#  Socket wrappers
# ------------------------------------------------------------------------------
class LocalListeningLogSocket(socket.socket):
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
class LocalForwardingLogSocket(socket.socket):
    # --------------------------------------------------------------------------
    def __init__(self, socket_path):
        self._socket_path = socket_path
        socket.socket.__init__(self, socket.AF_UNIX, socket.SOCK_STREAM)
        self.connect(self._socket_path)
        self.setblocking(False)

# ------------------------------------------------------------------------------
class NetworkForwardingLogSocket(socket.socket):
    # --------------------------------------------------------------------------
    def __init__(self, socket_addr):
        self._socket_host, self._socket_port = socket_addr.split(":")
        self._socket_port = int(self._socket_port)
        socket.socket.__init__(self, socket.AF_INET, socket.SOCK_STREAM)
        self.connect((self._socket_host, self._socket_port))
        self.setblocking(False)

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
        elif tag == "hb":
            self._tracker.heartbeat(self._src_id)
        elif tag == "i":
            pass
        elif tag == "c":
            pass
        elif tag == "ds":
            self._tracker.declareState(
                self._src_id,
                attr.get("src"),
                attr.get("tag"),
                attr.get("bgn"),
                attr.get("end"))
        elif tag == "as":
            self._tracker.activeState(
                self._src_id,
                attr.get("src"),
                attr.get("tag"))
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
        elif tag == "hb":
            self._tracker.heartbeat(self._src_id)

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
        self._tracker.finishLog(self._src_id, self._clean_shutdown)

# ------------------------------------------------------------------------------
class XmlLogClientHandler(xml.sax.ContentHandler):

    # --------------------------------------------------------------------------
    def __init__(self, options, processor, connection, selector):
        if options.local_socket:
            self._forward_socket = LocalForwardingLogSocket(options.local_socket)
        elif options.network_socket:
            self._forward_socket = NetworkForwardingLogSocket(options.network_socket)
        else:
            self._forward_socket = None
        self._log = logging.getLogger("eagiproc.xmlhandler")
        self._processor = processor
        self._connection = connection
        self._selector = selector
        self._buffer = bytes()

    # --------------------------------------------------------------------------
    def __del__(self):
        try: self._connection.close()
        except AttributeError: pass

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
                if self._forward_socket:
                    self._forward_socket.send(data)

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
                        self._log.error("failed to decode: '%s'" % line)
                        break
                    except:
                        self._log.error("failed to process XML '%s'" % line)
                        raise
                self._buffer = sep.join(lines[done:])
            else:
                self.disconnect()
        except socket.error:
            self.disconnect()

# ------------------------------------------------------------------------------
#  Progress tracking
# ------------------------------------------------------------------------------
class NoProgressTracker(object):
    # --------------------------------------------------------------------------
    def __init__(self):
        pass

    # --------------------------------------------------------------------------
    def pipelineProgress(self, pipeline):
        class _PipelineProgress(object):
            # ------------------------------------------------------------------
            def __init__(self, pipeline):
                pass

            # ------------------------------------------------------------------
            def instanceProgress(self, parent, instance):
                class _InstanceProgress(object):
                    # ----------------------------------------------------------
                    def __init__(self, instance):
                        pass

                    # ----------------------------------------------------------
                    def processProgress(self, parent, process):
                        class _ProcessProgress(object):
                            # --------------------------------------------------
                            def __init__(self, process):
                                pass

                            # --------------------------------------------------
                            def update(self, progress_info):
                                pass

                            # --------------------------------------------------
                            def onProcessExit(self, process):
                                pass

                        return _ProcessProgress(process)
                    # ----------------------------------------------------------
                    def onProcessExit(self, instance):
                        pass

                return _InstanceProgress(instance)

            # ------------------------------------------------------------------
            def instanceStarted(self, pipeline):
                pass

            # ------------------------------------------------------------------
            def instanceFinished(self, pipeline):
                pass

            # ------------------------------------------------------------------
            def finished(self, pipeline):
                pass

        return _PipelineProgress(pipeline)

# ------------------------------------------------------------------------------
class EnlightenProgressTracker(object):
    # --------------------------------------------------------------------------
    def __init__(self, manager):
        self._manager = manager

    # --------------------------------------------------------------------------
    def makeBar(self, total, desc):
        return self._manager.counter(
            total=total,
            bar_format=u"{desc}{desc_pad}{percentage:3.0f}%|{bar}|{elapsed}<{eta}",
            desc=desc,
            leave=False)

    # --------------------------------------------------------------------------
    def pipelineProgress(self, pipeline):
        class _PipelineProgress(object):
            # ------------------------------------------------------------------
            def __init__(self, parent, pipeline):
                self._parent = parent

            # ------------------------------------------------------------------
            def isTracking(self):
                return self._total is not None

            # ------------------------------------------------------------------
            def instanceProgress(self, parent, instance):
                class _InstanceProgress(object):
                    # ----------------------------------------------------------
                    def __init__(self, parent, instance):
                        self._parent = parent

                    # ----------------------------------------------------------
                    def processProgress(self, parent, process):
                        class _ProcessProgress(object):
                            # --------------------------------------------------
                            def __init__(self, parent, process):
                                self._parent = parent
                                self._desc = process.identity()
                                self._total = 1000
                                self._bar = None
                                self._val = 0.0
                                self._cur = 0

                            # --------------------------------------------------
                            def makeBar(self):
                                return self._parent._parent._parent.makeBar(
                                    self._total,
                                    self._desc)

                            # --------------------------------------------------
                            def update(self, progress_info):
                                value = progress_info["value"]
                                if self._val <= value:
                                    self._val = value
                                else:
                                    self._bar = None

                                if self._bar is None:
                                    self._cur = 0
                                    self._val = 0.0
                                    self._min = progress_info["min"]
                                    self._max = progress_info["max"]
                                    self._bar = self.makeBar()

                                pos = int(self._total*(value-self._min)/(self._max-self._min))
                                while self._cur < pos:
                                    try:
                                        self._cur += 1
                                        self._bar.update(force=(self._cur==pos))
                                    except: pass

                            # --------------------------------------------------
                            def onProcessExit(self, process):
                                if self._bar is not None:
                                    self._bar.close()

                        return _ProcessProgress(parent, process)
                    # ----------------------------------------------------------
                    def onProcessExit(self, instance):
                        pass

                return _InstanceProgress(parent, instance)

            # ------------------------------------------------------------------
            def instanceStarted(self, pipeline):
                pass

            # ------------------------------------------------------------------
            def instanceFinished(self, pipeline):
                pass

            # ------------------------------------------------------------------
            def finished(self, unused):
                pass

        return _PipelineProgress(self, pipeline)

# ------------------------------------------------------------------------------
def getProgressTracker(tracker, options):
    try:
        import enlighten
        return EnlightenProgressTracker(enlighten.get_manager())
    except ImportError:
        pass
    return NoProgressTracker()
# ------------------------------------------------------------------------------
#  Tracker
# ------------------------------------------------------------------------------
class ProcessLogTracker(object):
    # --------------------------------------------------------------------------

    # --------------------------------------------------------------------------
    def __init__(self, options, composition):
        self._log = logging.getLogger("eagiproc.tracker")
        self._options = options
        self._progress = getProgressTracker(self, options)
        self._composition = composition
        self._source_id = 0
        self._processes = {}
        self._process_number_to_src_id = {}
        self._state_begin_triggers = {}
        self._state_end_triggers = {}
        self._current_states = {}

        self._expect_clean_shutdown =\
            composition.config().cleanShutdownExpectations()
        self._expect_exit_code =\
            composition.config().exitCodeExpectations()
        self._expect_run_time =\
            composition.config().runTimeExpectations()
        self._expect_in_log_message =\
            composition.config().logMessageInfoExpectations()

        self._start_time = time.time()

    # --------------------------------------------------------------------------
    def log(self):
        return self._log

    # --------------------------------------------------------------------------
    def makePipelineProgress(self, pipeline):
        return self._progress.pipelineProgress(pipeline)

    # --------------------------------------------------------------------------
    def beginLog(self, src_id, info):
        if "session" in  info:
            number = int(info["identity"])
            self._state_begin_triggers[src_id] = {}
            self._state_end_triggers[src_id] = {}
            self._current_states[src_id] = {}
            proc_info = self._processes[src_id] = {}
            proc_info["identity"] = info["session"]
            proc_info["number"] = number
            proc_info["start"] = time.time()
            proc_info["update"] = time.time()
            self._process_number_to_src_id[number] = src_id
            self._log.info(
                "process %d '%s' started" % (
                src_id,
                proc_info["identity"]))

    # --------------------------------------------------------------------------
    def finishLog(self, src_id, clean_shutdown):
        try:
            remaining_states = self._current_states[src_id].copy()
            for source, source_states in remaining_states.items():
                for state_tag, state_infos in source_states.items():
                    for info in state_infos:
                        self.endState(src_id, source, info["instance"], state_tag)
        except KeyError: pass

        try:
            proc_info = self._processes[src_id]
            identity = proc_info["identity"]
            run_time = time.time() - proc_info["start"]

            try: self._expect_clean_shutdown[identity].update(clean_shutdown)
            except KeyError: pass
            try:
                for expectation in self._expect_run_time[identity]:
                    expectation.update(run_time)
            except KeyError: pass

            self._log.info(
                "process %d '%s' finished" % (
                src_id,
                proc_info["identity"]))
        except KeyError: pass

        try: del self._current_states[src_id]
        except KeyError: pass
        try: del self._state_begin_triggers[src_id]
        except KeyError: pass
        try: del self._state_end_triggers[src_id]
        except KeyError: pass
        try: del self._processes[src_id]
        except KeyError: pass

    # --------------------------------------------------------------------------
    def addMessage(self, src_id, info):
        try:
            source = info["source"]
            state_tag = self._state_begin_triggers[src_id][(source, info["tag"])]
            self.beginState(src_id, source, info["instance"], state_tag)
        except KeyError: pass

        try:
            message_info = None
            proc_info = self._processes[src_id]
            proc_info["update"] = time.time()
            msg_identity = proc_info["identity"]
            for expectation in self._expect_in_log_message[msg_identity]:
                if message_info is None:
                    src_states = self._current_states[src_id]
                    message_info = LogMessageInfo(info, src_states)
                expectation.update(message_info)
        except KeyError: pass

        def _get_progr_info(v):
            ktmap = [("min", float),("value",float),("max",float)]
            return {k: t(v[k]) for k, t in ktmap}

        try:
            for name, data in info["args"].items():
                for value in data["values"]:
                    if value["type"] == "MainPrgrss":
                        self.progress(src_id, _get_progr_info(value))
        except KeyError: pass

        try:
            source = info["source"]
            state_tag = self._state_end_triggers[src_id][(source, info["tag"])]
            self.endState(src_id, source, info["instance"], state_tag)
        except KeyError: pass

    # --------------------------------------------------------------------------
    def declareState(self, src_id, source, state_tag, begin_tag, end_tag):
        self._state_begin_triggers[src_id][(source, begin_tag)] = state_tag
        self._state_end_triggers[src_id][(source, end_tag)] = state_tag

    # --------------------------------------------------------------------------
    def activeState(self, src_id, source, state_tag):
        try:
            self._composition.addProcessActiveState(
                self._processes[src_id]["number"],
                (source, state_tag))
        except KeyError: pass

    # --------------------------------------------------------------------------
    def beginState(self, src_id, source, instance, state_tag):
        try:
            source_state = self._current_states[src_id][source]
        except KeyError:
            source_state = self._current_states[src_id][source] = {}

        try:
            states = source_state[state_tag]
        except KeyError:
            states = source_state[state_tag] = []

        now = time.time()
        states.append({
            "instance": instance,
            "begin": now
        })
        try:
            self._composition.handleProcessStateBegin(
                self._processes[src_id]["number"],
                (source, state_tag),
                now)
        except KeyError: pass

    # --------------------------------------------------------------------------
    def endState(self, src_id, source, instance, state_tag):
        now = time.time()
        try:
            src_states = self._current_states[src_id]
            states = src_states[source][state_tag]
            index = None
            for i in range(len(states)):
                if states[i]["instance"] == instance:
                    index = i
                    break
            if index is None:
                raise KeyError()

            finished_state = states[index]
            finished_state["duration"] = now - finished_state["begin"]

            self.handleFinishedState(src_id, src_states)

            try:
                self._composition.handleProcessStateEnd(
                    self._processes[src_id]["number"],
                    (source, state_tag),
                    now)
            except KeyError: pass

            del src_states[source][state_tag][index]
        except KeyError:
            self._log.warning(
                "%d ending unregistered state '%s'" % (
                src_id,
                state_tag))

    # --------------------------------------------------------------------------
    def handleFinishedState(self, src_id, info):
        # TODO
        pass

    # --------------------------------------------------------------------------
    def progress(self, src_id, info):
        try:
            self._composition.updateProcessProgress(
                self._processes[src_id]["number"],
                info)
        except KeyError: pass

    # --------------------------------------------------------------------------
    def heartbeat(self, src_id):
        try:
            proc_info = self._processes[src_id]
            proc_info["update"] = time.time()
        except KeyError: pass

    # --------------------------------------------------------------------------
    def makeProcessor(self):
        self._source_id += 1
        return XmlLogProcessor(self._source_id, self)

    # --------------------------------------------------------------------------
    def onProcessStateBegin(self, pipeline, instance_index, process, state, state_info):
        self._log.info(
            "process %d '%s' entered %s/%s %sstate" % (
            self._process_number_to_src_id[process.number()],
            pipeline.identity(),
            state[0],
            state[1],
            "active " if state_info.get("is_active", False) else ""))

    # --------------------------------------------------------------------------
    def onProcessStateEnd(self, pipeline, instance_index, process, state, state_info):
        self._log.info(
            "process %d '%s' left %s/%s state" % (
            self._process_number_to_src_id[process.number()],
            pipeline.identity(),
            state[0],
            state[1]))

    # --------------------------------------------------------------------------
    def onProcessProgressChange(self, pipeline, instance_index, process, progress_info):
        mn = progress_info["min"]
        vl = progress_info["value"]
        mx = progress_info["max"]
        self._log.debug(
            "process %d '%s' %.1f%% done" % (
            self._process_number_to_src_id[process.number()],
            pipeline.identity(),
            100.0 * (vl - mn) / (mx - mn)))

    # --------------------------------------------------------------------------
    def onProcessExit(self, pipeline, pipeline_index, process):
        try:
            self._expect_exit_code[process.identity()]\
                .update(process.identity(), process.exitCode())
        except KeyError: pass

    # --------------------------------------------------------------------------
    def onPipelineInstanceFinished(self, instance):
        self._log.info(
            "instance %d of pipeline '%s' finished" % (
            instance.index(),
            instance.identity()))

    # --------------------------------------------------------------------------
    def onPipelineFinished(self, pipeline):
        self._log.info("pipeline '%s' finished" % pipeline.identity())

    # --------------------------------------------------------------------------
    def allExpectations(self):
        for exp in self._expect_clean_shutdown.values():
            yield exp
        for exp in self._expect_exit_code.values():
            yield exp
        for exps in self._expect_run_time.values():
            for exp in exps:
                yield exp
        for exps in self._expect_in_log_message.values():
            for exp in exps:
                yield exp

    # --------------------------------------------------------------------------
    def result(self):
        success = True
        for expectation in self.allExpectations():
            success = expectation.check(self) and success
        return 0 if success else 1

# ------------------------------------------------------------------------------
#  Main loop
# ------------------------------------------------------------------------------
def manageProcesses(options, composition, tracker):
    with selectors.DefaultSelector() as selector:
        log_sock = LocalListeningLogSocket(options.logSocketPath())
        selector.register(
            log_sock,
            selectors.EVENT_READ,
            data = tracker
        )

        while composition.manage(tracker):
            events = selector.select(timeout=1.0)
            for key, mask in events:
                if type(key.data) is ProcessLogTracker:
                    connection, addr = log_sock.accept()
                    connection.setblocking(False)
                    selector.register(
                        connection,
                        selectors.EVENT_READ,
                        data = XmlLogClientHandler(
                            options,
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
    logging.basicConfig(
        encoding="utf-8",
        format='[%(asctime)s] %(levelname)s: %(message)s',
        level=logging.DEBUG if options.debug else logging.INFO)

    try:
        composition = PipelineComposition(options, PipelineConfig(options))
        if not options.dry_run:
            tracker = ProcessLogTracker(options, composition)
            composition.setTracker(tracker)

            return manageProcesses(options, composition, tracker)
    except Exception as error:
        logging.error(str(error))
        if options.debug:
            raise
    return 0

# ------------------------------------------------------------------------------
if __name__ == "__main__": sys.exit(main())

