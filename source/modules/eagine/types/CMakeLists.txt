# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(eagine.core.types PARTITION interface)
eagine_add_module(
	eagine.core.types PARTITION concepts
	IMPORTS std
)
eagine_add_module(
	eagine.core.types PARTITION function_constant
	IMPORTS std
)
eagine_add_module(
	eagine.core.types PARTITION bitfield
	IMPORTS std
)

eagine_add_module(
	eagine.core.types PARTITION basic
	IMPORTS std eagine.core.build_config
)

eagine_add_module(
	eagine.core.types PARTITION tribool
	IMPORTS std concepts
)

eagine_add_module(
	eagine.core.types PARTITION structural
	IMPORTS std
)

eagine_add_module(
	eagine.core.types PARTITION metaprogramming
	IMPORTS std basic
)


eagine_add_module(
	eagine.core.types PARTITION limits
	IMPORTS std basic
)

eagine_add_module(
	eagine.core.types PARTITION optional_like
	IMPORTS
		std basic tribool
		limits concepts
)

eagine_add_module(
	eagine.core.types PARTITION holder
	IMPORTS
		std basic tribool
		concepts optional_like
)

eagine_add_module(
	eagine.core.types PARTITION placeholder
	IMPORTS
		std tribool optional_like
)

eagine_add_module(
	eagine.core.types PARTITION outcome
	IMPORTS
		std basic limits concepts
)

eagine_add_module(
	eagine.core.types PARTITION compare
	IMPORTS std basic
)

eagine_add_module(
	eagine.core.types PARTITION integer_range
	IMPORTS std concepts limits
)

eagine_add_module(
	eagine.core.types PARTITION enum_map
	IMPORTS std basic bitfield
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
		optional_iterator
		optional_reference
		optional_like_tuple
		valid_if
		tribool
		holder
	IMPORTS
		std
)
