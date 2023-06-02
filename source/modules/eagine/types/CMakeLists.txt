# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(eagine.core.types PARTITION concepts)
eagine_add_module(eagine.core.types PARTITION function_constant)
eagine_add_module(eagine.core.types PARTITION interface)
eagine_add_module(eagine.core.types PARTITION bitfield)

eagine_add_module(
	eagine.core.types PARTITION basic
	IMPORTS eagine.core.build_config
)

eagine_add_module(
	eagine.core.types PARTITION tribool
	IMPORTS concepts
)

eagine_add_module(
	eagine.core.types PARTITION structural
)

eagine_add_module(
	eagine.core.types PARTITION metaprogramming
	IMPORTS basic
)


eagine_add_module(
	eagine.core.types PARTITION extract
	IMPORTS concepts
)

eagine_add_module(
	eagine.core.types PARTITION optional_reference
	IMPORTS
		basic tribool
		concepts
)

eagine_add_module(
	eagine.core.types PARTITION holder
	IMPORTS
		concepts optional_reference
)

eagine_add_module(
	eagine.core.types PARTITION placeholder
	IMPORTS
		tribool optional_reference
)

eagine_add_module(
	eagine.core.types PARTITION outcome
	IMPORTS
		extract basic
		concepts
)

eagine_add_module(
	eagine.core.types PARTITION compare
	IMPORTS basic
)

eagine_add_module(
	eagine.core.types PARTITION limits
	IMPORTS basic outcome
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
	IMPORTS
		eagine.core.build_config
)

eagine_add_module_tests(
	eagine.core.types
	UNITS
		anything
		bitfield
		enum_map
		placeholder
		integer_range
		metaprogramming
		nothing
		optional_reference
		tribool
		holder
)
