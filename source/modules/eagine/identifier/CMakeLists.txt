# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.identifier
	INTERFACES
		identifier
	SOURCES
		identifier
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.vectorization
)

eagine_add_module_tests(
	eagine.core.identifier
	UNITS
		identifier
	IMPORTS
		eagine.core.types
)
