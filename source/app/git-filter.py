#!/usr/bin/env python3
# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
#
# git config --global filter.eagine.clean "eagine-git-filter --clean"
# git config --global filter.eagine.smudge "eagine-git-filter --smudge"
#
# update entries in .gitattributes with filter=eagine
# for example: docker-compose.yml text eol=lf filter=eagine

import os
import sys
import csv
import argparse
import tempfile
import subprocess
# ------------------------------------------------------------------------------
class FilterArgParser(argparse.ArgumentParser):
    # --------------------------------------------------------------------------
    def __init__(self, **kw):
        argparse.ArgumentParser.__init__(self, **kw)

        self.add_argument(
            "--print-bash-completion",
            metavar='FILE|-',
            dest='print_bash_completion',
            default=None)

        self.add_argument(
            '--debug',
            action="store_true",
            default=False)

        self.add_argument(
            "--mapping",
            dest="mapping_path",
            action="store",
            metavar="PATH",
            type=os.path.realpath,
            default=os.path.expanduser("~/.config/eagine/git-filter.csv"),
            help="""Path to the secrets mapping CSV file""")

        action_group = self.add_mutually_exclusive_group()

        action_group.add_argument(
            "--clean",
            dest="action",
            action="store_const",
            const="clean",
            help="""Clean filter action""")

        action_group.add_argument(
            "--smudge",
            dest="action",
            action="store_const",
            const="smudge",
            help="""Smudge filter action""")

        self.add_argument(
            "input_paths",
            type=os.path.realpath,
            nargs=argparse.REMAINDER)

    # --------------------------------------------------------------------------
    def process_parsed_options(self, options):
        if options.print_bash_completion is None:
            if options.action is None:
                self.error("Either --clean or --smudge must be specified")
        return options

    # --------------------------------------------------------------------------
    def parse_args(self, args):
        return self.process_parsed_options(
            argparse.ArgumentParser.parse_args(self, args))

# ------------------------------------------------------------------------------
def make_argument_parser():
    return FilterArgParser(
        prog=os.path.basename(__file__),
        description="""
            EAGine Git filter helper script.
        """,
        epilog="""
            Copyright (c) 2024 Matúš Chochlík.
            Permission is granted to copy, distribute and/or modify this document
            under the terms of the Boost Software License, Version 1.0.
            (See a copy at http://www.boost.org/LICENSE_1_0.txt)
        """
    )

# ------------------------------------------------------------------------------
def names_and_secrets(options):
    with open(options.mapping_path) as csvfile:
        for row in csv.reader(csvfile, delimiter=',', quotechar='"'):
            try: yield row[0], row[1]
            except: pass

# ------------------------------------------------------------------------------
def apply_filter(options):
    with tempfile.NamedTemporaryFile(mode='w+t') as sedfd:
        for name, secret in names_and_secrets(options):
            if options.action == "smudge":
                sedfd.write(f"s/smudge_{name}/{secret}/g\n")
            elif options.action == "clean":
                sedfd.write(f"s/{secret}/smudge_{name}/g\n")
        sedfd.flush()

        if not options.input_paths:
            subprocess.call(["sed", "-f", sedfd.name])
        else:
            for path in options.input_paths:
                subprocess.call(["sed", "-f", sedfd.name, "-i", path])

# ------------------------------------------------------------------------------
#  bash completion
# ------------------------------------------------------------------------------
def printBashCompletion(argparser, options):
    from eagine.argparseUtil import printBashComplete
    def _printIt(fd):
        printBashComplete(
            argparser,
            "_eagine_git_filter",
            "eagine-git-filter",
            ["--print-bash-completion"],
            fd)
    if options.print_bash_completion == "-":
        _printIt(sys.stdout)
    else:
        with open(options.print_bash_completion, "wt") as fd:
            _printIt(fd)

# ------------------------------------------------------------------------------
#  main
# ------------------------------------------------------------------------------
def main():
    debug = False
    try:
        argparser = make_argument_parser()
        options = argparser.parse_args(sys.argv[1:])
        debug = options.debug
        if options.print_bash_completion:
            printBashCompletion(argparser, options)
            return 0
        else:
            apply_filter(options)
            return 0
    except Exception as err:
        if debug: raise
        else: print(err)
    return 1

# ------------------------------------------------------------------------------
if __name__ == "__main__":
    sys.exit(main())
    
