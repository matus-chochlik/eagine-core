# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.valid_if PARTITION compare
	IMPORTS
		std
		eagine.core.types
		eagine.core.reflection
)
eagine_add_module(
	eagine.core.valid_if PARTITION filesystem
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
)
eagine_add_module(
	eagine.core.valid_if PARTITION range
	IMPORTS
		std compare
		eagine.core.types
)

eagine_add_module(
	eagine.core.valid_if
	IMPORTS
		eagine.core.types
		eagine.core.reflection
)

eagine_add_module_tests(
	eagine.core.valid_if
	UNITS
		range
		filesystem
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
		eagine.core.reflection
)
