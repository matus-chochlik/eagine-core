#!/usr/bin/env python3
# coding=utf-8
# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
import os
import re
import sys
import argparse
import subprocess

# ------------------------------------------------------------------------------
class WorkflowArgParser(argparse.ArgumentParser):
    # --------------------------------------------------------------------------
    def __init__(self, **kw):
        argparse.ArgumentParser.__init__(self, **kw)

        self.add_argument(
            '--debug',
            action="store_true",
            default=False
        )

        self.add_argument(
            "--dry-run",
            default=True, #TODO
            action="store_true",
            help="""
                Only show the commands that should be executed
                but don't do anything.
            """
        )

        self.add_argument(
            "--remote",
            dest="git_remote",
            metavar="NAME",
            action="store",
            default="github",
            help="""
                Name of the git remote to pull from / push to.
            """
        )

        action_group = self.add_mutually_exclusive_group()

        action_group.add_argument(
            "--begin-release",
            dest="action",
            action="store_const",
            const="begin_release",
            help="""
                Starts a new release.
            """
        )
        action_group.add_argument(
            "--finish-release",
            dest="action",
            action="store_const",
            const="finish_release",
            help="""
                Finishes the current release.
            """
        )
        action_group.add_argument(
            "--begin-hotfix",
            dest="action",
            action="store_const",
            const="begin_hotfix",
            help="""
                Starts a new hotfix.
            """
        )
        action_group.add_argument(
            "--finish-hotfix",
            dest="action",
            action="store_const",
            const="finish_hotfix",
            help="""
                Finishes the current hotfix.
            """
        )

    # --------------------------------------------------------------------------
    def process_parsed_options(self, options):
        return options

    # --------------------------------------------------------------------------
    def parse_args(self, args):
        return self.process_parsed_options(
            argparse.ArgumentParser.parse_args(self, args)
        )

# ------------------------------------------------------------------------------
def make_argument_parser():
    return WorkflowArgParser(
            prog=os.path.basename(__file__),
            description="""
            EAGine Git workflow helper script
        """
    )
# ------------------------------------------------------------------------------
class Workflow(object):
    # --------------------------------------------------------------------------
    def __init__(self, options):
        self._options = options
        self._ownp = os.path.realpath(__file__)

    # --------------------------------------------------------------------------
    def own_path(self):
        return self._ownp

    # --------------------------------------------------------------------------
    def tool_path(self):
        return os.path.normpath(os.path.join(self.own_path(), os.path.pardir))

    # --------------------------------------------------------------------------
    def rel_path(self):
        return os.path.relpath(self.tool_path(), start=os.getcwd())

    # --------------------------------------------------------------------------
    def core_path(self):
        return os.path.normpath(os.path.join(self.tool_path(), os.path.pardir))

    # --------------------------------------------------------------------------
    def version_path(self):
        return os.path.join(self.core_path(), "VERSION")

    # --------------------------------------------------------------------------
    def read_version(self):
        with open(self.version_path(), "rt") as ver_fd:
            m = re.match(r"(\d+)\.(\d+)\.(\d+)", ver_fd.readline().strip())
            return int(m.group(1)), int(m.group(2)), int(m.group(3))

    # --------------------------------------------------------------------------
    def version_string(self, version_numbers):
        return "%d.%d.%d" % version_numbers

    # --------------------------------------------------------------------------
    def bumped_version_number(self, version_numbers, idx):
        return tuple(n if i < idx else n+1 if i== idx else 0 for i, n in enumerate(version_numbers))

    # --------------------------------------------------------------------------
    def next_repo_release(self):
        if self.is_in_core():
            return self.bumped_version_number(self.read_version(), 1)
        else:
            return self.read_version()

    # --------------------------------------------------------------------------
    def next_repo_hotfix(self):
        if self.is_in_core():
            return self.bumped_version_number(self.read_version(), 2)
        else:
            return self.read_version()

    # --------------------------------------------------------------------------
    def is_in_core(self):
        p = self.rel_path()
        if p == os.path.curdir:
            return True
        if p == "tools":
            return True
        if p.endswith(os.path.join(os.path.pardir, "tools")):
            return True
        return False

    # --------------------------------------------------------------------------
    def repo_root_path(self):
        if self.is_in_core():
            return self.core_path()
        p = self.rel_path()
        if p == "eagine-core/tools":
            return os.path.realpath(os.path.join(os.getcwd(), os.path.pardir))
        if p == "submodules/eagine-core/tools":
            return os.path.realpath(os.getcwd())
        if p.endswith("/submodules/eagine-core/tools"):
            return os.path.realpath(p.removesuffix("/submodules/eagine-core/tools"))

    # --------------------------------------------------------------------------
    def submodules_path(self):
        return os.path.normpath(os.path.join(self.repo_root_path(), "submodules"))

    # --------------------------------------------------------------------------
    def all_submodules(self):
        for ent in os.scandir(self.submodules_path()):
            if ent.is_dir():
                yield ent.path

    # --------------------------------------------------------------------------
    def own_submodules(self):
        for p in self.all_submodules():
            if os.path.basename(p).startswith("eagine-"):
                yield p

    # --------------------------------------------------------------------------
    def write_file(self, file_path, contents, simulate=None):
        if simulate is None:
            simulate = self._options.dry_run
        if simulate:
            print("echo -n '%s' > %s" % (contents, file_path))
        else:
            with open(file_path, "wt") as dst:
                dst.write(contents)

    # --------------------------------------------------------------------------
    def execute_command(self, cmd_line, simulate=None, simresult=str()):
        if simulate is None:
            simulate = self._options.dry_run
        if simulate:
            print(cmd_line[0]+" '"+str("' '").join(cmd_line[1:])+"'")
            return simresult
        else:
            proc = subprocess.Popen(
                cmd_line,
                cwd=self.repo_root_path(),
                stdout=subprocess.PIPE
            )
            # TODO: out, err = proc.communicate()
            return out.decode('utf-8')

    # --------------------------------------------------------------------------
    def git_command(self, args, simulate=None, simresult=str()):
        return self.execute_command(["git"]+args, simulate, simresult)

    # --------------------------------------------------------------------------
    def git_current_branch(self):
        return self.git_command(
            ["rev-parse", "--abbrev-ref", "HEAD"],
            root_dir,
            False
        ).strip()

    # --------------------------------------------------------------------------
    def begin_release(self):
        remote = self._options.git_remote
        self.git_command(["checkout", "develop"])
        self.git_command(["pull", remote, "develop"])
        next_version = self.version_string(self.next_repo_release())
        self.git_command(["checkout", "-b", "release-"+next_version, "develop"])
        if self.is_in_core():
            self.write_file(self.version_path(), next_version)
            self.git_command(["add", self.version_path()])
        self.git_command(["commit", "-m", "Started release-"+next_version])
        self.git_command(["push", remote, "release-"+next_version])

    # --------------------------------------------------------------------------
    def run(self):
        if self._options.debug:
            print(self._options)

        if self._options.action == "begin_release":
            self.begin_release()

# ------------------------------------------------------------------------------
def main():
    debug = False
    try:
        arg_parser = make_argument_parser()
        options = arg_parser.parse_args(sys.argv[1:])
        debug = options.debug
        Workflow(options).run()
    except Exception as err:
        print(err)
        if debug:
            raise
# ------------------------------------------------------------------------------
main()


