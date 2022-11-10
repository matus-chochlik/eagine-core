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
            default=False,
            action="store_true",
            help="""
                Only show the commands that should be executed
                but don't do anything.
            """
        )

        self.add_argument(
            "--branch",
            dest="git_branch",
            metavar="NAME",
            action="store",
            default="develop",
            help="""
                Name of the development git branch that is the source of the release.
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
            "--update-submodules",
            dest="action",
            action="store_const",
            const="update_submodules",
            help="""
                Updates the sub-modules of the current repository.
            """
        )
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
class WorkflowUIBase(object):
    # --------------------------------------------------------------------------
    def __init__(self, options):
        self._options = options
        self._commands = None

    # --------------------------------------------------------------------------
    def begin(self):
        # TODO
        # self._commands = []
        return self

    # --------------------------------------------------------------------------
    def commit(self, parent):
        self._commands = None
        return self

    # --------------------------------------------------------------------------
    def execute_command(self, parent, cmd_line, work_dir, simulate, simresult):
        if not self.should_execute(cmd_line, work_dir):
            raise RuntimeError("Command cancelled")
        if self._commands is None:
            parent.do_execute_command(cmd_line, work_dir, simulate, simresult)
        else:
            self._commands.append(
                lambda p: p.do_execute_command(cmd_line, work_dir, simulate, simresult))
        return self

    # --------------------------------------------------------------------------
    def write_file(self, parent, file_path, contents, simulate):
        if not self.should_write(file_path, contents):
            raise RuntimeError("Write cancelled")
        if self._commands is None:
            parent.do_write_file(file_path, contents, simulate)
        else:
            self._commands.append(
                lambda p: p.do_write_file(file_path, contents, simulate))
        return self


# ------------------------------------------------------------------------------
class WorkflowWhiptailUI(WorkflowUIBase):
    # --------------------------------------------------------------------------
    def __init__(self, options):
        WorkflowUIBase.__init__(self, options)

    # --------------------------------------------------------------------------
    def operation_failed(self, text):
        proc = subprocess.Popen(["whiptail", "--msgbox", text, "20", "74"])
        out, err = proc.communicate()

    # --------------------------------------------------------------------------
    def should_execute(self, cmd_line, work_dir):
        text = "execute:\n"
        text += " %s '%s'\n" % (cmd_line[0], "' '".join(cmd_line[1:]))
        text += "\nin directory:\n"
        text += " '%s'\n" % work_dir
        proc = subprocess.Popen([
            "whiptail",
            "--yes-button", "execute",
            "--no-button", "cancel",
            "--yesno", text, "20", "74"])
        out, err = proc.communicate()
        return proc.returncode == 0

    # --------------------------------------------------------------------------
    def should_write(self, file_path, contents):
        text = "write: '%s'" % contents
        text += "\ninto file:\n"
        text += " '%s'\n" % file_path 
        proc = subprocess.Popen([
            "whiptail",
            "--yes-button", "write",
            "--no-button", "cancel",
            "--yesno", text, "20", "74"])
        out, err = proc.communicate()
        return proc.returncode == 0


# ------------------------------------------------------------------------------
class WorkflowZenityUI(object):
    # --------------------------------------------------------------------------
    def __init__(self, options):
        self._options = options
        # TODO zenity

# ------------------------------------------------------------------------------
class WorkflowDialogUI(object):
    # --------------------------------------------------------------------------
    def __init__(self, options):
        self._options = options
        # TODO dialog

# ------------------------------------------------------------------------------
def make_ui(options):
    # TODO
    return WorkflowWhiptailUI(options)
# ------------------------------------------------------------------------------
class Workflow(object):
    # --------------------------------------------------------------------------
    def __init__(self, options):
        self._ui = make_ui(options)
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
    def release_branch(self, version_tag):
        return "release-" + version_tag

    # --------------------------------------------------------------------------
    def hotfix_branch(self, version_tag):
        return "hotfix-" + version_tag

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
    def do_write_file(self, file_path, contents, simulate=None):
        if simulate is None:
            simulate = self._options.dry_run
        if simulate:
            print("echo -n '%s' > %s" % (contents, file_path))
        else:
            with open(file_path, "wt") as dst:
                dst.write(contents)

    # --------------------------------------------------------------------------
    def write_file(self, file_path, contents, simulate=None):
        return self._ui.write_file(self, file_path, contents, simulate)

    # --------------------------------------------------------------------------
    def do_execute_command(self, cmd_line, work_dir, simulate=None, simresult=str()):
        def _cmd_str():
            return "%s '%s'" % (cmd_line[0], str("' '").join(cmd_line[1:]))
        if simulate is None:
            simulate = self._options.dry_run
        if simulate:
            if work_dir != self.repo_root_path():
                print("cd '%s' && " % work_dir, end='')
            print(_cmd_str())
            return simresult
        else:
            proc = subprocess.Popen(
                cmd_line,
                cwd=work_dir,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE
            )
            out, err = proc.communicate()
            if proc.returncode != 0:
                text = "command %s:\n\n%s" % (_cmd_str(), err.decode('utf-8'))
                self._ui.operation_failed(text)
                raise RuntimeError(text)
            return out.decode('utf-8')

    # --------------------------------------------------------------------------
    def begin_work(self):
        self._ui.begin()

    # --------------------------------------------------------------------------
    def commit(self):
        self._ui.commit(self)

    # --------------------------------------------------------------------------
    def execute_command(self, cmd_line, work_dir, simulate=None, simresult=str()):
        return self._ui.execute_command(self, cmd_line, work_dir, simulate, simresult)

    # --------------------------------------------------------------------------
    def git_dir_command(self, args, work_dir, simulate=None, simresult=str()):
        return self.execute_command(["git"] + args, work_dir, simulate, simresult)

    # --------------------------------------------------------------------------
    def git_command(self, args, simulate=None, simresult=str()):
        return self.git_dir_command(args, self.repo_root_path(), simulate, simresult)

    # --------------------------------------------------------------------------
    def do_git_dir_command(self, args, work_dir, simulate=None, simresult=str()):
        return self.do_execute_command(["git"] + args, work_dir, simulate, simresult)

    # --------------------------------------------------------------------------
    def do_git_command(self, args, simulate=None, simresult=str()):
        return self.do_git_dir_command(args, self.repo_root_path(), simulate, simresult)

    # --------------------------------------------------------------------------
    def git_current_branch(self):
        return self.do_git_command(
            ["rev-parse", "--abbrev-ref", "HEAD"],
            False
        ).strip()

    # --------------------------------------------------------------------------
    def git_has_branch(self, branch_name):
        return bool(self.do_git_command(
                ["branch", "--list", branch_name],
                False
            ).strip())

    # --------------------------------------------------------------------------
    def git_has_remote_branch(self, branch_name):
        self.do_git_command(["fetch", self._options.git_remote], False)
        return bool(self.do_git_command(
                ["branch", "--list", branch_name],
                False
            ).strip())

    # --------------------------------------------------------------------------
    def update_submodules(self):
        remote = self._options.git_remote
        branch = self._options.git_branch
        for moddir in self.own_submodules():
            self.git_dir_command(["checkout", branch], moddir)
            self.git_dir_command(["pull", remote, branch], moddir)

    # --------------------------------------------------------------------------
    def begin_release(self):
        remote = self._options.git_remote
        develop = self._options.git_branch
        self.begin_work()
        self.git_command(["checkout", develop])
        self.git_command(["pull", remote, develop])
        self.commit()
        release_tag = self.version_string(self.next_repo_release())
        release_branch = self.release_branch(release_tag)
        self.begin_work()
        self.git_command(["checkout", "-b", release_branch, develop])
        if self.is_in_core():
            self.write_file(self.version_path(), release_tag)
            self.git_command(["add", self.version_path()])
        else:
            self.update_submodules()
        self.git_command(["commit", "-m", "Started release "+release_tag])
        self.commit()
        self.git_command(["push", remote, release_branch])

    # --------------------------------------------------------------------------
    def finish_release(self):
        remote = self._options.git_remote
        develop = self._options.git_branch
        release_tag = self.version_string(self.read_version())
        release_branch = self.release_branch(release_tag)
        if self.git_current_branch() != release_branch:
            release_tag = self.version_string(self.next_repo_release())
            release_branch = self.release_branch(release_tag)
            if self.git_has_branch(release_branch):
                self.git_command(["checkout", release_branch])
                self.git_command(["pull", remote, release_branch])
            elif self.git_has_remote_branch(release_branch):
                self.git_command(["checkout", "-b", release_branch, remote+"/"+release_branch])
                self.git_command(["pull", remote, release_branch])
            else: raise RuntimeError(
                "Release branch '"+release_branch+"' does not exist. "
                "Re-run with --begin-release to start a new release."
            )
        self.git_command(["checkout", release_branch])
        self.git_command(["pull", remote, release_branch])
        self.git_command(["checkout", "main"])
        self.git_command(["pull", remote, "main"])
        self.git_command(["merge", "-X", "theirs", "--no-ff", release_branch])
        self.git_command(["tag", "-a", release_tag, "-m", "Tagged release "+release_tag])
        self.git_command(["checkout", develop])
        self.git_command(["pull", remote, develop])
        self.git_command(["merge", "--no-ff", release_branch])
        self.git_command(["push", remote, release_tag])
        self.git_command(["push", remote, "main"])
        self.git_command(["push", remote, develop])
        self.git_command(["push", remote, ":"+release_branch])
        self.git_command(["branch", "-D", release_branch])

    # --------------------------------------------------------------------------
    def begin_hotfix(self):
        remote = self._options.git_remote
        self.begin_work()
        self.git_command(["checkout", "main"])
        self.git_command(["pull", remote, "main"])
        self.commit()
        source_tag = self.version_string(self.read_version())
        hotfix_tag = self.version_string(self.next_repo_hotfix())
        hotfix_branch = self.hotfix_branch(hotfix_tag)
        self.git_command(["checkout", "-b", hotfix_branch, source_tag+"^2"])
        if self.is_in_core():
            self.write_file(self.version_path(), hotfix_tag)
            self.git_command(["add", self.version_path()])
        else:
            self.update_submodules()
        self.git_command(["commit", "-m", "Started hotfix "+hotfix_tag])
        self.git_command(["push", remote, hotfix_branch])


    # --------------------------------------------------------------------------
    def finish_hotfix(self):
        remote = self._options.git_remote
        develop = self._options.git_branch
        hotfix_tag = self.version_string(self.read_version())
        hotfix_branch = self.hotfix_branch(hotfix_tag)
        print(self.git_current_branch(), hotfix_branch)
        if self.git_current_branch() != hotfix_branch:
            hotfix_tag = self.version_string(self.next_repo_hotfix())
            hotfix_branch = self.hotfix_branch(hotfix_tag)
            if self.git_has_branch(hotfix_branch):
                self.git_command(["checkout", hotfix_branch])
                self.git_command(["pull", remote, hotfix_branch])
            elif self.git_has_remote_branch(hotfix_branch):
                self.git_command(["checkout", "-b", hotfix_branch, remote+"/"+hotfix_branch])
                self.git_command(["pull", remote, hotfix_branch])
            else: raise RuntimeError(
                "Hotfix branch '"+hotfix_branch+"' does not exist. "
                "Re-run with --begin-hotfix to start a new hotfix."
            )
        self.git_command(["checkout", "main"])
        self.git_command(["pull", remote, "main"])
        self.git_command(["merge", "-X", "theirs", "--no-ff", hotfix_branch])
        self.git_command(["tag", "-a", hotfix_tag, "-m", "Tagged hotfix "+hotfix_tag])
        self.git_command(["checkout", develop])
        self.git_command(["pull", remote, develop])
        self.git_command(["merge", "-X", "theirs", "--no-ff", hotfix_branch])
        self.git_command(["push", remote, hotfix_tag])
        self.git_command(["push", remote, "main"])
        self.git_command(["push", remote, develop])
        self.git_command(["push", remote, ":"+hotfix_branch])
        self.git_command(["branch", "-D", hotfix_branch])


    # --------------------------------------------------------------------------
    def run(self):
        if self._options.debug:
            print(self._options)

        if self._options.action == "update_submodules":
            self.update_submodules()
        elif self._options.action == "begin_release":
            self.begin_release()
        elif self._options.action == "finish_release":
            self.finish_release()
        elif self._options.action == "begin_hotfix":
            self.begin_hotfix()
        elif self._options.action == "finish_hotfix":
            self.finish_hotfix()

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


