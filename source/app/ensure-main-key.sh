#!/bin/bash
# coding=utf-8
# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt
#
function eagine_has_main_key() {
	[[ -f "${1}" ]] && grep -qx 'main_key: .\{512,\}' "${1}"
}

size=${1:-512}
defaults="${HOME}/.config/eagine/defaults.yaml"
mkdir -p "$(dirname ${defaults})" &&
eagine_has_main_key "${defaults}" ||\
echo "main_key: $(dd if=/dev/random status=none bs=${size} count=1 | base64 -w 0)" >> "${defaults}"

