# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.valid_if
	PARTITION decl
	IMPORTS
		eagine.core.types
		eagine.core.concepts
)

eagine_add_module(
	eagine.core.valid_if PARTITION always
	IMPORTS decl eagine.core.concepts
)
eagine_add_module(
	eagine.core.valid_if PARTITION never
	IMPORTS decl eagine.core.concepts
)
eagine_add_module(
	eagine.core.valid_if PARTITION not_empty
	IMPORTS decl eagine.core.concepts
)
eagine_add_module(
	eagine.core.valid_if PARTITION compare
	IMPORTS decl eagine.core.concepts
)
eagine_add_module(
	eagine.core.valid_if PARTITION indicated
	IMPORTS decl eagine.core.concepts
)
eagine_add_module(
	eagine.core.valid_if PARTITION filesystem
	IMPORTS decl
		eagine.core.concepts
		eagine.core.memory
)
eagine_add_module(
	eagine.core.valid_if PARTITION range
	IMPORTS
		decl compare
		eagine.core.concepts
		eagine.core.types
)

eagine_add_module(
	eagine.core.valid_if
	INTERFACES
		valid_if
	PARTITIONS
		decl
		always
		never
		compare
		not_empty
		indicated
		filesystem
		range
)

eagine_add_module_tests(
	eagine.core.valid_if
	UNITS
		decl
)
