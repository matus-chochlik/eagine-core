#!/bin/bash -e
# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
#
PGPASSWORD=$(< "${HOME}/.config/eagine/eagilog/psql/secrets/eagilog_pg.passwd") \
psql \
	-h localhost \
	-d eagilog \
	-U eagilog \
	"${@}"
