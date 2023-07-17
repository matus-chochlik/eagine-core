# coding=utf-8
# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
#
import os, sys 
# ------------------------------------------------------------------------------
def getGitConfigValue(section, option):
        try:
            import gitconfig
            key = "%(section)s.%(option)s" % {
                "section": section,
                "option" : option
            }
            return gitconfig.GitConfig()[key]
        except: pass

        try:
            import git
            path = [os.path.normpath(os.path.expanduser("~/.gitconfig"))]
            return git.GitConfigParser(path, read_only=True).get_value(section, option)
        except: raise
# ------------------------------------------------------------------------------
def adjustAttributes(command_name, attribs):
    import datetime

    attribs["date"] = str(datetime.datetime.now().date())
    attribs["command"] = command_name
    attribs["command_uc"] = command_name.upper()

    if not "brief" in attribs:
        attribs["brief"] = command_name

    if not "heading" in attribs:
        attribs["heading"] = attribs["brief"]

    if not "description" in attribs:
        attribs["description"] = command_name


    if not "author_name" in attribs:
        try:
            attribs["author_name"] = get_git_config_value('user', 'name')
        except: pass

    if not "author_email" in attribs:
        try:
            attribs["author_email"] = get_git_config_value('user', 'email')
        except: pass

    if not "author_name" in attribs:
        try:
            import getpass
            attribs["author_name"] = getpass.getuser()
        except: pass

    if not "author_name" in attribs:
        attribs["author_name"] = "Matus Chochlik"

    if not "author_email" in attribs:
        attribs["author_email"] = "chochlik@gmail.com"

    return attribs
# ------------------------------------------------------------------------------
def printBashComplete(
    argparser,
    function_name,
    command_name,
    skip_opts,
    output=sys.stdout):

    def _printLine(linestr):
        output.write(linestr)
        output.write(os.linesep)

    import argparse

    _printLine('#  Distributed under the Boost Software License, Version 1.0.')
    _printLine('#  (See accompanying file LICENSE_1_0.txt or copy at')
    _printLine('#  http://www.boost.org/LICENSE_1_0.txt)')
    _printLine('#')
    _printLine('#  Automatically generated file. Do NOT modify manually,')
    _printLine('#  edit %(self)s instead' % {"self" : os.path.basename(sys.argv[0])})
    _printLine(str())
    _printLine('function %(function_name)s()' % {
        "function_name": function_name
    })
    _printLine('{')
    _printLine('    COMPREPLY=()')
    _printLine('    local curr="${COMP_WORDS[COMP_CWORD]}"')
    _printLine('    local prev="${COMP_WORDS[COMP_CWORD-1]}"')
    _printLine(str())

    options_with_path=list()
    for action in argparser._actions:
        if action.type == os.path.abspath:
            options_with_path += action.option_strings

    _printLine('    case "${prev}" in')
    _printLine('        -h|--help)')
    _printLine('            return 0;;')
    if len(options_with_path) > 0:
        _printLine('        %s)' % str("|").join(options_with_path))
        _printLine('            COMPREPLY=($(compgen -f "${curr}"))')
        _printLine('            return 0;;')

    for action in argparser._actions:
        if action.choices is not None:
            _printLine('        %s)' % str("|").join(action.option_strings))
            ch = str(" ").join([str(c) for c in action.choices])
            _printLine('            COMPREPLY=($(compgen -W "%s" -- "${curr}"))' % ch)
            _printLine('            return 0;;')

    _printLine('        *)')
    _printLine('    esac')
    _printLine(str())

    _printLine('    local only_once_opts=" \\')
    for action in argparser._actions:
        if type(action) != argparse._AppendAction:
            option_strings = [opt for opt in action.option_strings if opt not in skip_opts]
            _printLine('        %s \\' % str(" ").join(option_strings))
    _printLine('    "')
    _printLine(str())

    meog_list = list()
    meog_id = 0
    for group in argparser._mutually_exclusive_groups:
        _printLine('    local meog_%d=" \\' % meog_id)
        for action in group._group_actions:
            _printLine('        %s \\' % str(" ").join(action.option_strings))
        _printLine('    "')
        _printLine(str())
        meog_list.append('meog_%d' % meog_id)
        meog_id += 1

    _printLine('    local repeated_opts=" \\')
    for action in argparser._actions:
        if type(action) == argparse._AppendAction:
            _printLine('        %s \\' % str(" ").join(action.option_strings))
    _printLine('    "')
    _printLine(str())
    _printLine('    local opts="${repeated_opts}"')
    _printLine(str())
    _printLine('    for opt in ${only_once_opts}')
    _printLine('    do')
    _printLine('        local opt_used=false')
    _printLine('        for pre in ${COMP_WORDS[@]}')
    _printLine('        do')
    _printLine('            if [ "${opt}" == "${pre}" ]')
    _printLine('            then opt_used=true && break')
    _printLine('            fi')
    _printLine('        done')
    _printLine('        if [ "${opt_used}" == "false" ]')
    _printLine('        then')
    if len(meog_list) > 0:
        _printLine('            for meog in "${%s}"' % str('}" "${').join(meog_list))
        _printLine('            do')
        _printLine('                local is_meo=false')
        _printLine('                for meo in ${meog}')
        _printLine('                do')
        _printLine('                    if [ "${opt}" == "${meo}" ]')
        _printLine('                    then is_meo=true && break')
        _printLine('                    fi')
        _printLine('                done')
        _printLine('                if [ "${is_meo}" == "true" ]')
        _printLine('                then')
        _printLine('                    for pre in ${COMP_WORDS[@]}')
        _printLine('                    do')
        _printLine('                        for meo in ${meog}')
        _printLine('                        do')
        _printLine('                            if [ "${pre}" == "${meo}" ]')
        _printLine('                            then opt_used=true && break')
        _printLine('                            fi')
        _printLine('                        done')
        _printLine('                    done')
        _printLine('                fi')
        _printLine('            done')
        _printLine(str())
    _printLine('            if [ "${opt_used}" == "false" ]')
    _printLine('            then opts="${opts} ${opt}"')
    _printLine('            fi')
    _printLine('        fi')
    _printLine('    done')
    _printLine(str())
    _printLine('    if [ ${COMP_CWORD} -le 1 ]')
    _printLine('    then opts="--help ${opts}"')
    _printLine('    fi')
    _printLine(str())
    _printLine('    COMPREPLY=($(compgen -W "${opts}" -- "${curr}"))')
    _printLine('} && complete -F %(function_name)s %(command_name)s' % {
        "function_name": function_name,
        "command_name": command_name
    })
    _printLine('# vi: syntax=bash')
# ------------------------------------------------------------------------------
def printManual(
    argparser,
    command_name,
    attributes = dict(),
    output=sys.stdout):
    def _printLine(linestr):
        output.write(linestr)
        output.write(os.linesep)

    import argparse

    attributes = adjust_attributes(command_name, attributes)

    _printLine('.TH %(command_uc)s 1 "%(date)s" "%(heading)s"' % attributes)
    _printLine('.SH "NAME"')
    _printLine('%(command)s \\- %(brief)s' % attributes)
    _printLine('.SH "SYNOPSIS"')
    _printLine('.B %(command)s' % attributes)
    _printLine('[')
    _printLine('OPTIONS')
    _printLine(']')

    _printLine('.SH "DESCRIPTION"')
    _printLine('%(description)s' % attributes)

    _printLine('.SH "OPTIONS"')

    for action in argparser._actions:
        _printLine(".TP")
        opt_info = str()
        for opt_str in action.option_strings:
            if opt_info:
                opt_info += ", "
            opt_info += '\\fB'+opt_str+'\\fR'
            if action.type == os.path.abspath:
                opt_info += ' <\\fI'+str(action.dest).upper()+'\\fR>';
            if action.choices is not None:
                opt_info += ' {\\fB'
                opt_info += '\\fR,\\fB'.join(map(str, action.choices))
                opt_info += '\\fR}'
        _printLine(opt_info)
        _printLine(output,
            str(' ').join(action.help.split()) % {
                "prog": "\\f%(command)s\\fR" % attributes,
                "default": "\\fB"+str(action.default)+"\\fR"
            }
        )

    _printLine('.SH "AUTHOR"')
    _printLine('%(author_name)s, %(author_email)s' % attributes)

    _printLine('.SH "COPYRIGHT"')
    _printLine('Copyright (c) %(author_name)s' % attributes)

    _printLine(".PP")
    _printLine("Permission is granted to copy, distribute and/or modify this document")
    _printLine("under the terms of the Boost Software License, Version 1.0.")
    _printLine("(See a copy at http://www.boost.org/LICENSE_1_0.txt)")
# ------------------------------------------------------------------------------
