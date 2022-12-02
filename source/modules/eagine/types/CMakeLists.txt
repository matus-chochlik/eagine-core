# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(eagine.core.types PARTITION basic)
eagine_add_module(eagine.core.types PARTITION tribool)
eagine_add_module(eagine.core.types PARTITION interface)
eagine_add_module(eagine.core.types PARTITION bitfield)

eagine_add_module(
	eagine.core.types PARTITION structural
)

eagine_add_module(
	eagine.core.types PARTITION nothing
	IMPORTS eagine.core.build_config
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
	eagine.core.types PARTITION optional_ref
	IMPORTS
		nothing tribool
		eagine.core.concepts
)

eagine_add_module(
	eagine.core.types PARTITION outcome
	IMPORTS
		extract nothing
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
	eagine.core.types PARTITION integer
	IMPORTS limits
)

eagine_add_module(
	eagine.core.types PARTITION selector
	IMPORTS basic
)

eagine_add_module(
	eagine.core.types PARTITION enum_map
	IMPORTS bitfield selector
)

eagine_add_module(
	eagine.core.types
	INTERFACES types
	PARTITIONS
		basic
		nothing
		anything
		structural
		interface
		compare
		limits
		integer
		integer_range
		extract
		tribool
		optional_ref
		outcome
		bitfield
		function_constant
		selector
		enum_map
		metaprogramming
)

eagine_add_module_tests(
	eagine.core.types
	UNITS
		anything
		bitfield
		tribool
)
