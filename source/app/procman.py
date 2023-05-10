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
import random
import argparse
import tempfile
import subprocess

# ------------------------------------------------------------------------------
class ArgumentParser(argparse.ArgumentParser):
    # --------------------------------------------------------------------------
    def user_config_path(self, arg):
        return os.path.join(
            os.path.expanduser("~"),
            ".config",
            "eagine",
            arg)

    # --------------------------------------------------------------------------
    def config_search_paths(self, arg):
        return [
            os.path.abspath(arg),
            self.user_config_path(arg)
        ]

    # --------------------------------------------------------------------------
    def config_search_exts(self):
        return [".eagiproc", ".json"]

    # --------------------------------------------------------------------------
    def find_config_path(self, arg):
        for path in self.config_search_paths(arg):
            for ext in self.config_search_exts():
                tmp = path + ext
                if os.path.isfile(tmp):
                    return tmp

    # --------------------------------------------------------------------------
    def config_basename(self, path):
        return os.path.splitext(os.path.basename(path))[0]

    # --------------------------------------------------------------------------
    def find_config_names(self):
        result = []
        for path in self.config_search_paths(os.path.curdir):
            for ext in self.config_search_exts():
                for filepath in glob.glob(os.path.join(path, "*"+ext)):
                    yield filepath

    # --------------------------------------------------------------------------
    def __init__(self, **kw):
        argparse.ArgumentParser.__init__(self, **kw)

        def config_path(arg):
            result = self.find_config_path(arg)
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
                "paths": "', `".join(self.config_search_paths("config")),
                "exts": "', `".join(self.config_search_exts()),
                "configs": "', `".join([self.config_basename(x) for x in self.find_config_names()])
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
            "--print-config", "-P",
            action="store_true",
            default=False,
            help="""Prints the fully loaded and merged process configuration."""
        )

    # --------------------------------------------------------------------------
    def process_parsed_options(self, options):
        return options


    # --------------------------------------------------------------------------
    def parse_args(self):
        # ----------------------------------------------------------------------
        class _Options(object):
            # ------------------------------------------------------------------
            def __init__(self, base):
                self.__dict__.update(base.__dict__)
                self._hashes = set()
                self._work_dir = tempfile.TemporaryDirectory(prefix="eagi")

            # ------------------------------------------------------------------
            def __del__(self):
                self._work_dir.cleanup()

            # ------------------------------------------------------------------
            def _gen_hash(self):
                return "%032x" % random.getrandbits(128)

            # ------------------------------------------------------------------
            def unique_hash(self):
                while True:
                    h = self._gen_hash()
                    if h not in self._hashes:
                        self._hashes.add(h)
                        return h

            # ------------------------------------------------------------------
            def work_dir(self):
                return self._work_dir.name

            # ------------------------------------------------------------------
            def log_socket_path(self):
                return os.path.join(self.work_dir(), "log.socket")

        # ----------------------------------------------------------------------
        return _Options(self.process_parsed_options(
            argparse.ArgumentParser.parse_args(self)
        ))

# ------------------------------------------------------------------------------
def get_argument_parser():
    return ArgumentParser(
        prog=os.path.basename(__file__),
        description="""launches and manages a group of processes"""
    )

# ------------------------------------------------------------------------------
class ExpansionRegExprs(object):
    # --------------------------------------------------------------------------
    def __init__(self, options):
        self._options = options
        self.commands = [
            ("pathid",
                re.compile(".*(\$\(pathid ([^)]+)\)).*"),
                lambda mtch : self._resolve_cmd_pathid(mtch.group(2))
            ),
            ("basename",
                re.compile(".*(\$\(basename ([^)]+)\)).*"),
                lambda mtch : self._resolve_cmd_basename(mtch.group(2))
            ),
            ("dirname",
                re.compile(".*(\$\(dirname ([^)]+)\)).*"),
                lambda mtch : self._resolve_cmd_dirname(mtch.group(2))
            ),
            ("wildcard",
                re.compile(".*(\$\(wildcard ([^)]+)\)).*"),
                lambda mtch : self._resolve_cmd_wildcard(mtch.group(2))
            ),
            ("which",
                re.compile(".*(\$\(which ([^)]+)\)).*"),
                lambda mtch : self._resolve_cmd_which(mtch.group(2))
            ),
            ("eagiapp",
                re.compile(".*(\$\(eagiapp ([^)]+)\)).*"),
                lambda mtch : self._resolve_cmd_eagiapp(mtch.group(2))
            ),
            ("uid",
                re.compile(".*(\$\(uid\)).*"),
                lambda mtch : self._resolve_cmd_uid()
            )
        ]

        self.adjustments = [
            ("instance",
                re.compile(".*(\$<timestamp>).*"),
                lambda inst, mtch: time.time()
            ),
            ("instance",
                re.compile(".*(\$<instance>).*"),
                lambda inst, mtch: inst.instance_id()
            ),
            ("uid",
                re.compile(".*(\$<uid>).*"),
                lambda inst, mtch: self._resolve_cmd_uid()
            )
        ]

        self.variable = re.compile(".*(\${([A-Za-z][A-Za-z_0-9]*)}).*")
        self.list_exp = re.compile(".*(\$\[([A-Za-z][A-Za-z_0-9]*)\.\.\.\]).*")
        self.sbsc_exp = re.compile(".*(\$\[([A-Za-z][A-Za-z_0-9]*)\[([0-9]+)\]\]).*")
        self.eval_exp = re.compile(".*(\$\(([0-9+*/%-]*)\)).*")

    # --------------------------------------------------------------------------
    def _resolve_cmd_basename(self, name):
        return os.path.basename(name)

    # --------------------------------------------------------------------------
    def _resolve_cmd_dirname(self, name):
        return os.path.dirname(name)

    # --------------------------------------------------------------------------
    def _resolve_cmd_pathid(self, name):
        return int(hash(name))

    # --------------------------------------------------------------------------
    def _resolve_cmd_wildcard(self, pattern):
        return [os.path.realpath(x) for x in glob.glob(pattern)]

    # --------------------------------------------------------------------------
    def _resolve_cmd_which(self, name):
        search_dirs = os.environ.get("PATH", "").split(':')
        search_dirs += os.path.dirname(__file__)
        for dir_path in search_dirs:
            cmd_path = os.path.join(dir_path, name)
            if os.path.isfile(cmd_path):
                if stat.S_IXUSR & os.stat(cmd_path)[stat.ST_MODE]:
                    return cmd_path
        return name

    # --------------------------------------------------------------------------
    def _resolve_cmd_eagiapp(self, name):
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
            yield self._resolve_cmd_wildcard(name)

        for found in _search():
            if found:
                return [found, "--use-asio-log", self._options.log_socket_path()]

    # --------------------------------------------------------------------------
    def _resolve_cmd_uid(self):
        return self._options.unique_hash()

# ------------------------------------------------------------------------------
def merge_configs(source, destination):
    for key, value in source.items():
        if isinstance(value, dict):
            node = destination.setdefault(key, {})
            merge_configs(value, node)
        elif isinstance(value, list):
            node = destination.setdefault(key, [])
            node += value
        else:
            destination[key] = value

    return destination

# ------------------------------------------------------------------------------
class PipelineConfig(object):
    # --------------------------------------------------------------------------
    def __init__(self, options):
        self._res = ExpansionRegExprs(options)
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

                        self.full_config = merge_configs(
                            partial_config,
                            self.full_config)
                except IOError as io_error:
                    print("error reading '%s': %s" % (config_path, io_error))
                except ValueError as value_error:
                    print("error parsing '%s': %s" % (config_path, value_error))

        for pipeline in self.full_config.get("pipelines", []):
            variables = {}
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
        if name in ["SELF"]:
            return os.path.abspath(__file__)

        if name in ["TEMPDIR"]:
            return tempfile.gettempdir()

        if name in ["HOME"]:
            return os.path.expanduser("~")

        return name

    # --------------------------------------------------------------------------
    def _do_sub_vars(self, expr, variables):
        if type(expr) is not str:
            yield expr
            return

        value = str(variables.get(expr, os.environ.get(expr, self._fallback(expr))))
        while True:
            found = re.match(self._res.variable, value)
            if found:
                prev = value[:found.start(1)]
                folw = value[found.end(1):]
                for repl in self._do_substitute(
                    found.group(2),
                    variables):
                    yield prev + repl + folw
                break
            else:
                yield value
                break

    # --------------------------------------------------------------------------
    def _do_sub_evals(self, expr, variables):
        for value in self._do_sub_vars(expr, variables):
            if type(value) is not str:
                yield value
            else:
                while True:
                    found = re.match(self._res.eval_exp, value)
                    if found:
                        prev = value[:found.start(1)]
                        repl = str(eval(found.group(2)))
                        folw = value[found.end(1):]
                        yield prev + repl + folw
                        break;
                    else:
                        yield value
                        break

    # --------------------------------------------------------------------------
    def _do_apply_cmds(self, expr, variables, cmds):
        if not cmds:
            for value in self._do_sub_evals(expr, variables):
                yield value
        else:
            cmd_name, cmd_re, cmd_func = cmds[0]
            for value in self._do_apply_cmds(expr, variables, cmds[1:]):
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
                            yield prev + item + folw
                    else:
                        yield value

    # --------------------------------------------------------------------------
    def _do_substitute(self, expr, variables):
        for value in self._do_apply_cmds(expr, variables, self._res.commands):
            yield value

    # --------------------------------------------------------------------------
    def _substitute(self, exprs, variables):
        if type(exprs) is not list:
            exprs = [exprs]

        for expr in exprs:
            for value in self._do_substitute(expr, variables):
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
                            if type(repl) is not list:
                                repl = [repl]

                            try:
                                elem = repl[int(found.group(3)) % len(repl)]

                                for nested_value in self._substitute(
                                    [prev+str(elem)+folw],
                                    variables):
                                    yield nested_value
                            except ValueError:
                                yield value
                        else:
                            yield value

    # --------------------------------------------------------------------------
    def resolve(self, exprs, variables):
        return list(self._substitute(exprs, variables))

    # --------------------------------------------------------------------------
    def _do_apply_adjs(self, inst, expr, variables, adjs):
        if not adjs:
            yield expr
        else:
            adj_name, adj_re, adj_func = adjs[0]
            for value in self._do_apply_adjs(inst, expr, variables, adjs[1:]):
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
    def adjust_arg_of(self, instance, arg, variables):
        for value in self._substitute(
            list(self._do_apply_adjs(
                instance, arg, variables, self._res.adjustments)),
            variables):
            yield value

    # --------------------------------------------------------------------------
    def pipeline_configs(self):
        return self.full_config.get("pipelines", [])

# ------------------------------------------------------------------------------
class PipelineInstance(object):
    # --------------------------------------------------------------------------
    def __init__(self, parent, counter):
        self._parent = parent
        self._counter = counter
        self._args = parent.command_line_of(self)

    # --------------------------------------------------------------------------
    def instance_id(self):
        return self._counter

    # --------------------------------------------------------------------------
    def command_line(self):
        return list(self._args)

# ------------------------------------------------------------------------------
class Pipeline(object):
    # --------------------------------------------------------------------------
    def __init__(self, options, parent, config):
        self._parent = parent
        self._config = config
        self._instance_counter = 0
        self._instance = None

    # --------------------------------------------------------------------------
    def _prepare_args(self, instance, args):
        variables = self._config.get("variables", {})
        for arg in args:
            for adjusted in self._parent.adjust_arg_of(instance, arg, variables):
                yield adjusted

    # --------------------------------------------------------------------------
    def command_line_of(self, instance):
        commands = self._config["commands"]
        for args in commands:
            for adjusted in self._prepare_args(instance, args):
                yield adjusted

    # --------------------------------------------------------------------------
    def temp(self):
        self._instance = PipelineInstance(self, self._instance_counter)
        print(self._instance.command_line())
        self._instance_counter += 1

# ------------------------------------------------------------------------------
class PipelineComposition(object):
    # --------------------------------------------------------------------------
    def __init__(self, options, config):
        self._config = config
        self._pipelines = [
            Pipeline(options, self, cfg)
            for cfg in config.pipeline_configs()]

    # --------------------------------------------------------------------------
    def adjust_arg_of(self, instance, arg, variables):
        for value in self._config.adjust_arg_of(instance, arg, variables):
            yield value

    # --------------------------------------------------------------------------
    def temp(self):
        for p in self._pipelines:
            p.temp()

# ------------------------------------------------------------------------------
def manage_processes(options, composition):
    # TODO
    for i in range(10):
        composition.temp()

    return 0

# ------------------------------------------------------------------------------
def main():
    argparser = get_argument_parser()
    options = argparser.parse_args()
    composition = PipelineComposition(options, PipelineConfig(options))

    sys.exit(manage_processes(options, composition))

# ------------------------------------------------------------------------------
if __name__ == "__main__": main()

