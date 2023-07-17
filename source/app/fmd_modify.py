#!/usr/bin/env python3
# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt

import os
import re
import sys
import json
import shutil
import argparse
import tempfile

# ------------------------------------------------------------------------------
# Argument parsing
# ------------------------------------------------------------------------------
class ArgumentParser(argparse.ArgumentParser):
    # -------------------------------------------------------------------------
    def __init__(self, **kw):
        self._path_re = re.compile("^[A-Za-z_][A-Za-z0-9_]*(\.[A-Za-z_][A-Za-z0-9_]*)*$")

        def _valid_input_path(x):
            try:
                p = os.path.realpath(x)
                assert os.path.isfile(p)
                assert os.access(p, os.R_OK)
                return p
            except:
                self.error("`%s' is not an existing readable file" % str(x))

        def _valid_addition(x):
            if not self._path_re.match(x):
                try:
                    return int(x)
                except ValueError:
                    try:
                        return float(x)
                    except ValueError:
                        if x in ["null", "Null", "NULL"]:
                            return None
                        if x in ["true", "True", "TRUE"]:
                            return True
                        if x in ["false", "False", "FALSE"]:
                            return True
            return x

        def _valid_deletion(x):
            try:
                assert self._path_re.match(x)
                return x
            except:
                self.error("invalid attribute specifier '%s'" % s)

        argparse.ArgumentParser.__init__(self, **kw)

        self.add_argument(
            "--print-bash-completion",
            metavar='FILE|-',
            dest='print_bash_completion',
            default=None
        )

        self.add_argument(
            "--add", "-a",
            metavar=('ATTRIBUTE','VALUE'),
            dest='additions',
            nargs=2,
            type=_valid_addition,
            action="append",
            default=[]
        )

        self.add_argument(
            "--delete", "-d",
            metavar='ATTRIBUTE',
            dest='deletions',
            nargs='?',
            type=_valid_deletion,
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

        options.additions = [(_processKey(k), v) for k, v in options.additions]
        options.deletions = [_processKey(k) for k in options.deletions]
        return options
    # -------------------------------------------------------------------------
    def parseArgs(self):
        return self.processParsedOptions(argparse.ArgumentParser.parse_args(self))
# ------------------------------------------------------------------------------
def getArgumentParser():
    return ArgumentParser(
        prog=os.path.basename(__file__),
        description="""
            Modifies metadata in EAGine files with JSON header.
        """
    )
# ------------------------------------------------------------------------------
def updateHeader(options, header):
    for attrib in options.deletions:
        node = header
        try:
            for key in attrib[:-1]:
                node = node[key]
            del node[attrib[-1]]
        except KeyError:
            pass
                
    for attrib, value in options.additions:
        node = header
        for key in attrib[:-1]:
            node = node.setdefault(key, {})
        node[attrib[-1]] = value
    return header
# ------------------------------------------------------------------------------
def formatHeader(options, header, ofd):
    def _write(s):
        ofd.write(s.encode("utf-8"))
    first = True
    for key, value in header.items():
        if first:
            _write("{")
            first = False
        else:
            _write(",")
        _write(json.dumps(key))
        _write(':')
        if isinstance(value, dict):
            _write(json.dumps(value))
        else:
            _write(json.dumps(value))
        _write('\n')
    _write("}")
# ------------------------------------------------------------------------------
def processFile(options, path):
    with open(path, "rb") as ifd:
        decoder = json.JSONDecoder()
        head_bytes = bytes()
        while True:
            chunk = ifd.read(1024)
            if not chunk:
                break
            head_bytes += chunk
            try:
                data = bytes()
                head_str = head_bytes.decode("utf-8")
            except UnicodeDecodeError as ude:
                data = head_bytes[ude.start:]
                head_bytes = head_bytes[:ude.start]
                head_str = head_bytes.decode("utf-8")
            try:
                header, pos = decoder.raw_decode(head_str)
                data = head_str[pos:].encode("utf-8") + data
                break
            except json.JSONDecodeError as jde:
                pass
        with tempfile.NamedTemporaryFile(mode='wb', delete=False) as ofd:
            formatHeader(options, updateHeader(options, header), ofd)
            ofd.write(data)
            while True:
                chunk = ifd.read(4096)
                if not chunk:
                    break
                ofd.write(chunk)
        shutil.move(ofd.name, path)

# ------------------------------------------------------------------------------
def processFiles(options):
    for path in options.input_paths:
        processFile(options, path)
# ------------------------------------------------------------------------------
def printBashCompletion(argparser, options):
    from eagine.argparseUtil import printBashComplete
    def _printIt(fd):
        printBashComplete(
            argparser,
            "_eagine_fmd_modify",
            "eagine-fmd-modify",
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
        else:
            processFiles(options)
        return 0
    except Exception as error:
        print(type(error), error)
        raise
        return 1

# ------------------------------------------------------------------------------
if __name__ == '__main__':
    sys.exit(main())
