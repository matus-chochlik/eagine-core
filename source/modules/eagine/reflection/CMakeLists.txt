# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.reflection PARTITION decl_name
	IMPORTS
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.reflection PARTITION enumerators
	IMPORTS
		decl_name
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.reflection PARTITION data_members
	IMPORTS
		decl_name
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.reflection
	INTERFACES
		reflection
	PARTITIONS
		decl_name
		enumerators
		data_members
)

eagine_add_module_tests(
	eagine.core.reflection
	UNITS
		enumerators
	IMPORTS
		eagine.core.concepts
		eagine.core.types
		eagine.core.memory
)

