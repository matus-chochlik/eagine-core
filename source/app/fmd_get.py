#!/usr/bin/env python3
# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt

import os
import re
import sys
import json
import argparse

# ------------------------------------------------------------------------------
# Argument parsing
# ------------------------------------------------------------------------------
class ArgumentParser(argparse.ArgumentParser):
    # --------------------------------------------------------------------------
    def __init__(self, **kw):
        self._path_re = re.compile("^[A-Za-z_][A-Za-z0-9_]*(\.[A-Za-z_][A-Za-z0-9_]*)*$")
        self._qry_re = re.compile("^([A-Za-z_][A-Za-z0-9_]*(\.[A-Za-z_][A-Za-z0-9_]*)*)(==|!=|<=|>=|<|>|\))(.*)$")

        # ----------------------------------------------------------------------
        def _valid_input_path(x):
            try:
                p = os.path.realpath(x)
                assert os.path.isfile(p)
                assert os.access(p, os.R_OK)
                return p
            except:
                self.error("`%s' is not an existing readable file" % str(x))
        # ----------------------------------------------------------------------
        def _valid_attribute(x):
            try:
                assert self._path_re.match(x)
                return x
            except:
                self.error("invalid attribute specifier '%s'" % x)
        # ----------------------------------------------------------------------
        def _valid_query(x):
            def _value(v):
                try:
                    return int(v)
                except ValueError:
                    try:
                        return float(v)
                    except ValueError:
                        if v in ["null", "Null", "NULL"]:
                            return None
                        if v in ["true", "True", "TRUE"]:
                            return True
                        if v in ["false", "False", "FALSE"]:
                            return True
                return v

            def _operation(o):
                if o == "==":
                    return lambda x,y: x == y
                if o == "!=":
                    return lambda x,y: x != y
                if o == "<=":
                    return lambda x,y: x <= y
                if o == ">=":
                    return lambda x,y: x >= y
                if o == "<":
                    return lambda x,y: x < y
                if o == ">":
                    return lambda x,y: x > y
                if o == ")":
                    return lambda x,y: y in x

            try:
                found = self._qry_re.match(x)
                assert found
                attrib = found.group(1)
                operation = _operation(found.group(3))
                value = _value(found.group(4))
                return (attrib, operation, value)
            except Exception as e:
                print(e)
                self.error("invalid query specifier '%s'" % x)
        # ----------------------------------------------------------------------
        argparse.ArgumentParser.__init__(self, **kw)

        self.add_argument(
            "--print-bash-completion",
            metavar='FILE|-',
            dest='print_bash_completion',
            default=None
        )

        self.add_argument(
            "--value", "-v",
            metavar='ATTRIBUTE',
            dest='queried_values',
            nargs='?',
            type=_valid_attribute,
            action="append",
            default=[]
        )

        self.add_argument(
            "--query", "-q",
            metavar='ATTRIBUTE',
            dest='queries',
            nargs='?',
            type=_valid_query,
            action="append",
            default=[]
        )

        self.add_argument(
            "--input", "-i",
            metavar='INPUT-FILE',
            dest='input_paths',
            nargs='?',
            type=_valid_input_path,
            action="append",
            default=[]
        )

        self.add_argument(
            "_input_paths",
            metavar='INPUT-FILE',
            nargs=argparse.REMAINDER,
            type=_valid_input_path
        )

    # -------------------------------------------------------------------------
    def processParsedOptions(self, options):
        options.input_paths = set(options.input_paths)
        options.input_paths.update(set(options._input_paths))
        del options._input_paths
        options.input_paths = list(options.input_paths)

        def _processKey(k):
            try:
                assert isinstance(k, str)
                return k.split(".")
            except:
                self.error("invalid attribute specifier '%s'" % k)

        options.queries = [(_processKey(k), o, v) for k,o,v in options.queries]
        options.queried_values = [_processKey(k) for k in options.queried_values]

        return options
    # -------------------------------------------------------------------------
    def parseArgs(self):
        return self.processParsedOptions(argparse.ArgumentParser.parse_args(self))
# ------------------------------------------------------------------------------
def getArgumentParser():
    return ArgumentParser(
        prog=os.path.basename(__file__),
        description="""
            Queries metadata in EAGine files with JSON header.
        """
    )
# ------------------------------------------------------------------------------
def extractHeader(options, path):
    with open(path, "rb") as ifd:
        decoder = json.JSONDecoder()
        head_bytes = bytes()
        while True:
            chunk = ifd.read(1024)
            if not chunk:
                break
            head_bytes += chunk
            try:
                head_str = head_bytes.decode("utf-8")
            except UnicodeDecodeError as ude:
                head_bytes = head_bytes[:ude.start]
                head_str = head_bytes.decode("utf-8")
            try:
                header, pos = decoder.raw_decode(head_str)
                return header
            except json.JSONDecodeError as jde:
                pass
    return {}

# ------------------------------------------------------------------------------
def getAttribValue(header, attrib):
    node = header
    for key in attrib[:-1]:
        node = node.setdefault(key, {})
    return node[attrib[-1]]

# ------------------------------------------------------------------------------
def processFile(options, path):
    header = extractHeader(options, path)
    first = True
    if len(options.queried_values) > 0:
        for attrib in options.queried_values:
            if first:
                first = False
            else:
                sys.stdout.write('\t')
            sys.stdout.write(getAttribValue(header, attrib))
        sys.stdout.write('\n')

    for attrib, operation, value in options.queries:
        if not operation(getAttribValue(header, attrib), value):
            return False
    return True
# ------------------------------------------------------------------------------
def processFiles(options):
    for path in options.input_paths:
        if not processFile(options, path):
            return 1
    return 0
# ------------------------------------------------------------------------------
def printBashCompletion(argparser, options):
    from eagine.argparseUtil import printBashComplete
    def _printIt(fd):
        printBashComplete(
            argparser,
            "_eagine_fmd_get",
            "eagine-fmd-get",
            ["--print-bash-completion"],
            fd)
    if options.print_bash_completion == "-":
        _printIt(sys.stdout)
    else:
        with open(options.print_bash_completion, "wt") as fd:
            _printIt(fd)

# ------------------------------------------------------------------------------
#  Main function
# ------------------------------------------------------------------------------
def main():
    try:
        argparser = getArgumentParser()
        options = argparser.parseArgs()
        if options.print_bash_completion:
            printBashCompletion(argparser, options)
            return 0
        else:
            return processFiles(options)
    except Exception as error:
        print(type(error), error)
        raise
    return 2

# ------------------------------------------------------------------------------
if __name__ == '__main__':
    sys.exit(main())
