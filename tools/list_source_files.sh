#!/bin/bash
# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt

find "${1:-.}" -type f |\
grep -v -e '/submodules/' -e '/.git/' |\
xargs file |\
grep -i -e 'C++ source' -e 'C source' |\
cut -d: -f1 |\
grep -e '\.hpp$' -e '\.inl$' -e '\.cpp$'
