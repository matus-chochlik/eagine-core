# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.build_info
	SOURCES
		build_info
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
		eagine.core.valid_if
	PRIVATE_LINK_LIBRARIES
		eagine-core-generated-headers
)

eagine_add_module_tests(
	eagine.core.build_info
	UNITS
		build_info
	IMPORTS
		std
)
