#!/bin/bash
# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt
#
this_dir="$(realpath $(dirname ${0}))"
user_site="$(python3 -m site --user-site)"
if [[ ! -d "${user_site}" ]]
then mkdir -p "${user_site}"
fi

cp -r -f "${this_dir}/eagine" "${user_site}"
