# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.types PARTITION basic
	IMPORTS eagine.core.build_config
)
eagine_add_module(
	eagine.core.types PARTITION tribool
	IMPORTS eagine.core.concepts
)
eagine_add_module(eagine.core.types PARTITION interface)
eagine_add_module(eagine.core.types PARTITION bitfield)
eagine_add_module(eagine.core.types PARTITION placeholder)

eagine_add_module(
	eagine.core.types PARTITION structural
)

eagine_add_module(
	eagine.core.types PARTITION metaprogramming
	IMPORTS basic
)


eagine_add_module(
	eagine.core.types PARTITION extract
	IMPORTS eagine.core.concepts
)

eagine_add_module(
	eagine.core.types PARTITION optional_reference
	IMPORTS
		basic tribool
		eagine.core.concepts
)

eagine_add_module(
	eagine.core.types PARTITION outcome
	IMPORTS
		extract basic
		eagine.core.concepts
		eagine.core.types
)

eagine_add_module(
	eagine.core.types PARTITION compare
	IMPORTS basic
)

eagine_add_module(
	eagine.core.types PARTITION limits
	IMPORTS basic
)

eagine_add_module(
	eagine.core.types PARTITION integer_range
	IMPORTS limits
)

eagine_add_module(
	eagine.core.types PARTITION enum_map
	IMPORTS basic bitfield
)

eagine_add_module(
	eagine.core.types
	INTERFACES types
	PARTITIONS
		basic
		structural
		interface
		compare
		limits
		integer_range
		extract
		tribool
		optional_reference
		outcome
		bitfield
		placeholder
		function_constant
		enum_map
		metaprogramming
)

eagine_add_module_tests(
	eagine.core.types
	UNITS
		anything
		bitfield
		enum_map
		integer_range
		metaprogramming
		nothing
		optional_reference
		tribool
)
