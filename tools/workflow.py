#!/usr/bin/env python3
# coding=utf-8
# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
import os
import sys

# globally enables/disables the "dry-run" mode
dry_run = False

# returns a normalized path to the project root directory
def get_root_dir():

    return os.path.normpath(
        os.path.join(
            os.path.dirname(sys.argv[0]),
            os.path.pardir
        )
    )

# returns the path of the VERSION file
def version_file(root_dir = get_root_dir()):
    return os.path.join(root_dir, "VERSION")

# returns the current version numbers in a list
def current_version(root_dir = get_root_dir()):
    with open(version_file(root_dir), "rt") as src:
        return map(int, src.read().strip().split("."))

# returns the current version string
def version_string(version_numbers):
    return "%d.%d.%d" % tuple(version_numbers)

def bumped_version_number(version_numbers, idx):

    return [n if i < idx else n+1 if i== idx else 0 for i, n in enumerate(version_numbers)]

def next_release(root_dir = get_root_dir()):
    return bumped_version_number(current_version(root_dir), 1)

def next_hotfix(root_dir = get_root_dir()):
    return bumped_version_number(current_version(root_dir), 2)


# writes contents into a file
def write_file(file_path, contents, simulate=None):

    if simulate is None: simulate = dry_run
    if simulate:
        print("echo '-n' '"+contents+"' > "+file_path)
    else:
        with open(file_path, "wt") as dst:
            dst.write(contents)

# executes a command in a subprocess
def execute_command(cmd_line, root_dir=get_root_dir(), simulate=None):
    import subprocess

    if simulate is None: simulate = dry_run
    if simulate:
        print(cmd_line[0]+" '"+str("' '").join(cmd_line[1:])+"'")
        return str()
    else:
        proc = subprocess.Popen(
            cmd_line,
            cwd=root_dir,
            stdout=subprocess.PIPE
        )
        out, err = proc.communicate()
        return out.decode('utf-8')

# executes a git command
def git_command(parameters, root_dir=get_root_dir(), simulate=None):
    return execute_command(["git"]+parameters, root_dir, simulate)

# returns the name of the current git branch
def git_current_branch(root_dir=get_root_dir()):
    return git_command(
        ["rev-parse", "--abbrev-ref", "HEAD"],
        root_dir,
        False
    ).strip()

# returns true if the specified branch exists
def git_has_branch(branch_name, root_dir=get_root_dir):
    return bool(
        git_command(
            ["branch", "--list", branch_name],
            root_dir,
            False
        ).strip()
    )


# returns true if the specified remote branch exists
def git_has_remote_branch(branch_name, root_dir=get_root_dir):
    git_command(["fetch", "origin"], root_dir, False)
    return bool(
        git_command(
            ["branch", "--list", branch_name],
            root_dir,
            False
        ).strip()
    )

# Begins a new release
def action_begin_release():
    root_dir = get_root_dir()
    git_command(["checkout", "develop"], root_dir)
    git_command(["pull", "origin", "develop"], root_dir)
    next_version = version_string(next_release(root_dir))
    git_command(["checkout", "-b", "release-"+next_version, "develop"], root_dir)
    write_file(version_file(root_dir), next_version)
    git_command(["add", version_file(root_dir)], root_dir)
    git_command(["commit", "-m", "Started release-"+next_version], root_dir)
    git_command(["push", "origin", "release-"+next_version], root_dir)

# Finishes a release
def action_finish_release():
    root_dir = get_root_dir()
    current_branch = git_current_branch(root_dir)
    release_version = version_string(current_version(root_dir))
    release_branch = "release-"+release_version
    if current_branch != release_branch:
        release_version = version_string(next_release(root_dir))
        release_branch = "release-"+release_version
        if git_has_branch(release_branch, root_dir):
            git_command(["checkout", release_branch], root_dir)
            git_command(["pull", "origin", release_branch], root_dir)
        elif git_has_remote_branch(release_branch, root_dir):
            git_command(["checkout", "-b", release_branch, "origin/"+release_branch], root_dir)
            git_command(["pull", "origin", release_branch], root_dir)
        else: raise RuntimeError(
            "Release branch '"+release_branch+"' does not exist. "
            "Re-run with --begin-release to start a new release."
        )
    git_command(["checkout", "main"], root_dir)
    git_command(["pull", "origin", "main"], root_dir)
    git_command(["merge", "-X", "theirs", "--no-ff", release_branch], root_dir)
    git_command(["tag", "-a", release_version, "-m", "Tagged release "+release_version], root_dir)
    git_command(["checkout", "develop"], root_dir)
    git_command(["pull", "origin", "develop"], root_dir)
    git_command(["merge", "--no-ff", release_branch], root_dir)
    git_command(["branch", "-D", release_branch], root_dir)
    git_command(["push", "origin", ":"+release_branch], root_dir)
    git_command(["push", "origin", release_version], root_dir)
    git_command(["push", "origin", "main"], root_dir)
    git_command(["push", "origin", "develop"], root_dir)


# Begins a new hotfix
def action_begin_hotfix():
    root_dir = get_root_dir()
    git_command(["checkout", "main"], root_dir)
    git_command(["pull", "origin", "main"], root_dir)
    base_version = version_string(current_version(root_dir))
    next_version = version_string(next_hotfix(root_dir))
    git_command(["checkout", "-b", "hotfix-"+next_version, base_version+"^2"], root_dir)
    write_file(version_file(root_dir), next_version)
    git_command(["add", version_file(root_dir)], root_dir)
    git_command(["commit", "-m", "Started hotfix-"+next_version])
    git_command(["push", "origin", "hotfix-"+next_version], root_dir)

# Finishes a hotfix
def action_finish_hotfix():
    root_dir = get_root_dir()
    current_branch = git_current_branch(root_dir)
    hotfix_version = version_string(current_version(root_dir))
    hotfix_branch = "hotfix-"+hotfix_version
    if current_branch != hotfix_branch:
        hotfix_version = version_string(next_hotfix(root_dir))
        hotfix_branch = "hotfix-"+hotfix_version
        if git_has_branch(hotfix_branch, root_dir):
            git_command(["checkout", hotfix_branch], root_dir)
            git_command(["pull", "origin", hotfix_branch], root_dir)
        elif git_has_remote_branch(hotfix_branch, root_dir):
            git_command(["checkout", "-b", hotfix_branch, "origin/"+hotfix_branch], root_dir)
            git_command(["pull", "origin", hotfix_branch], root_dir)
        else: raise RuntimeError(
            "Hotfix branch '"+hotfix_branch+"' does not exist. "
            "Re-run with --begin-hotfix to start a new hotfix."
        )
    git_command(["checkout", "main"], root_dir)
    git_command(["pull", "origin", "main"], root_dir)
    git_command(["merge", "-X", "theirs", "--no-ff", hotfix_branch], root_dir)
    git_command(["tag", "-a", hotfix_version, "-m", "Tagged hotfix "+hotfix_version], root_dir)
    git_command(["checkout", "develop"], root_dir)
    git_command(["pull", "origin", "develop"], root_dir)
    git_command(["merge", "--no-ff", hotfix_branch], root_dir)
    git_command(["branch", "-D", hotfix_branch], root_dir)
    git_command(["push", "origin", ":"+hotfix_branch], root_dir)
    git_command(["push", "origin", hotfix_version], root_dir)
    git_command(["push", "origin", "main"], root_dir)
    git_command(["push", "origin", "develop"], root_dir)


# creates the command line argument parser
def get_argument_parser():
    import argparse

    argparser = argparse.ArgumentParser(
        prog="workflow",
        description="""
            Git Workflow utility script
        """,
        epilog="""
            Copyright (c) Matúš Chochlík.
            Permission is granted to copy, distribute and/or modify this document
            under the terms of the Boost Software License, Version 1.0.
            (See a copy at http://www.boost.org/LICENSE_1_0.txt)
        """
    )

    argparser.add_argument(
        "--dry-run",
        default=False,
        action="store_true",
        help="""
            Only print the commands that should be executed
            but don't do anything.
        """
    )

    argparser_action_group = argparser.add_mutually_exclusive_group()

    argparser_action_group.add_argument(
        "--begin-release",
        dest="action",
        action="store_const",
        const=action_begin_release,
        help="""
            Starts a new release.
        """
    )
    argparser_action_group.add_argument(
        "--finish-release",
        dest="action",
        action="store_const",
        const=action_finish_release,
        help="""
            Finishes the current release.
        """
    )
    argparser_action_group.add_argument(
        "--begin-hotfix",
        dest="action",
        action="store_const",
        const=action_begin_hotfix,
        help="""
            Starts a new hotfix.
        """
    )
    argparser_action_group.add_argument(
        "--finish-hotfix",
        dest="action",
        action="store_const",
        const=action_finish_hotfix,
        help="""
            Finishes the current hotfix.
        """
    )

    return argparser

def main():
    try:
        # parse and process the command-line arguments
        argparser = get_argument_parser()
        options = argparser.parse_args()

        global dry_run
        dry_run = options.dry_run

        if options.action:
            options.action()
        else: print("No action specified")
    except RuntimeError as rte:
        print("Runtime error: " + str(rte))

if __name__ == "__main__": main()

