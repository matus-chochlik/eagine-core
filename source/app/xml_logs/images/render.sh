#!/bin/bash
# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
for svg in $(dirname ${0})/src/*.svg
do inkscape \
	--pipe \
	--export-area-page \
	--export-overwrite \
	--export-type=png \
	--export-filename=$(dirname ${0})/$(basename ${svg} .svg).png \
	< ${svg}
done
