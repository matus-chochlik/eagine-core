#!/bin/bash -e
# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt
#
PGPASSWORD=$(< "${HOME}/.config/eagine/eagilog/secrets/eagilog_postgres.passwd") \
psql \
	-h localhost \
	-d eagilog \
	-U eagilog \
	"${@}"
