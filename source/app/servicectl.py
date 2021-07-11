#!/usr/bin/env python3
# coding=utf-8
# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt

import os
import re
import sys
import glob
import subprocess

def scan_services():
    services = {}
    config_dir = os.path.join(
        os.path.expanduser("~"),
        ".config",
        "eagine",
        "service")
    config_re = re.compile(config_dir + os.sep + "(.*)\\.txt")

    for service_group_config in glob.glob(os.path.join(config_dir, "*.txt")):
        config_match = config_re.match(service_group_config)
        group_name = config_match[1]
        try: service_group = services[group_name]
        except KeyError:
            service_group = services[group_name] = {}
        
        with open(service_group_config, 'rt') as cfg_file:
            for cfg_line in cfg_file.readlines():
                cfg_entry = cfg_line.split(":")
                service_name = cfg_entry[0].strip()
                if service_name:
                    try: service_entry = service_group[service_name]
                    except KeyError:
                        service_entry = service_group[service_name] = {}
                    service_entry["description"] =\
                        ":".join([x.strip() for x in cfg_entry[1:]])
                    service_entry["is_active"] =\
                        subprocess.call([
                            "systemctl",
                            "is-active",
                            "--user",
                            "--quiet",
                            "eagine-%s-%s" % (group_name, service_name)]) == 0
    services = {k: v for k, v in services.items() if len(v) > 0}
    return services

def change_setup_whiptail(setup):
    size = os.get_terminal_size()
    options = sum(len(g) for g in setup.values())
    whiptail_args = [
        "whiptail",
        "--notags",
        "--checklist",
        "Manage active services",
        str(int(1 + max(max(24, size.lines) / 2, options + 2))),
        str(int(1 + max(80, size.columns) / 2)),
        str(options)
    ]
    for group_name, group in setup.items():
        for service_name, service in group.items():
            whiptail_args += [
                "%s-%s" % (group_name, service_name),
                service["description"],
                "on" if service["is_active"] else "off"
            ]
    try:
        unused, selected = subprocess.Popen(
            whiptail_args,
            stderr=subprocess.PIPE,
            universal_newlines=True).communicate()
        active = [x.strip('"') for x in selected.split(" ")]
        for group_name, group in setup.items():
            for service_name, service in group.items():
                service["activate"] = "%s-%s" % (group_name, service_name) in active
    except subprocess.CalledProcessError:
        pass
    return setup

def change_setup_zenity(setup):
    zenity_args = [
        "zenity",
        "--list",
        "--checklist",
        "--title=Services",
        "--text=Manage active services",
        "--print-column=2",
        "--hide-column=2",
        "--column=Use",
        "--column=Description",
        "--column=Id"
    ]
    for group_name, group in setup.items():
        for service_name, service in group.items():
            zenity_args += [
                "TRUE" if service["is_active"] else "FALSE",
                "%s-%s" % (group_name, service_name),
                service["description"]
            ]
    try:
        selected, unused = subprocess.Popen(
            zenity_args,
            stdout=subprocess.PIPE,
            universal_newlines=True).communicate()
        active = selected.strip().split("|")
        for group_name, group in setup.items():
            for service_name, service in group.items():
                service["activate"] = "%s-%s" % (group_name, service_name) in active
    except subprocess.CalledProcessError:
        pass
    return setup

def change_setup(setup):
    if "--zenity" in sys.argv[1:]:
        return change_setup_zenity(setup)
    return change_setup_whiptail(setup)

def apply_setup(setup):
    actions = {
        "start": [],
        "stop": []
    }
    activated = []
    deactivated = []
    for group_name, group in setup.items():
        for service_name, service in group.items():
            if service["is_active"] and not service["activate"]:
                actions["stop"].append((group_name, service_name))
            elif not service["is_active"] and service["activate"]:
                actions["start"].append((group_name, service_name))

    for action, services in actions.items():
        if services:
            systemctl_args =\
                ["systemctl", "--user", action] +\
                ["eagine-%s-%s" % (g, s) for g, s in services]
            subprocess.check_call(systemctl_args)

def main():
    apply_setup(change_setup(scan_services()))

main()
