# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.identifier
	COMPONENT core-dev
	SOURCES
		identifier
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
		eagine.core.simd)

eagine_add_module_tests(
	eagine.core.identifier
	UNITS
		identifier
	IMPORTS
		std
		eagine.core.types)
