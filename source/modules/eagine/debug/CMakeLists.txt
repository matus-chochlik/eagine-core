# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.debug PARTITION valgrind
	IMPORTS eagine.core.types
)

eagine_add_module(
	eagine.core.debug PARTITION slow_exec
	IMPORTS valgrind eagine.core.types
)

eagine_add_module(
	eagine.core.debug
	INTERFACES
		debug
	PARTITIONS
		type_name
		valgrind
		slow_exec
	SOURCES
		type_name_impl
)

