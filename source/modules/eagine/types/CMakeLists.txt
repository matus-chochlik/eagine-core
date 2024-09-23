# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.types
	COMPONENT core-dev
	PARTITION interface)
eagine_add_module(
	eagine.core.types
	COMPONENT core-dev
	PARTITION concepts
	IMPORTS std)

eagine_add_module(
	eagine.core.types
	COMPONENT core-dev
	PARTITION bitfield
	IMPORTS std)

eagine_add_module(
	eagine.core.types
	COMPONENT core-dev
	PARTITION basic
	IMPORTS std eagine.core.build_config)

eagine_add_module(
	eagine.core.types
	COMPONENT core-dev
	PARTITION tribool
	IMPORTS std concepts)

eagine_add_module(
	eagine.core.types
	COMPONENT core-dev
	PARTITION structural
	IMPORTS std)

eagine_add_module(
	eagine.core.types
	COMPONENT core-dev
	PARTITION metaprogramming
	IMPORTS std basic)

eagine_add_module(
	eagine.core.types
	COMPONENT core-dev
	PARTITION function_constant
	IMPORTS
		std basic)

eagine_add_module(
	eagine.core.types
	COMPONENT core-dev
	PARTITION limits
	IMPORTS std basic)

eagine_add_module(
	eagine.core.types
	COMPONENT core-dev
	PARTITION simd_data
	IMPORTS std basic)

eagine_add_module(
	eagine.core.types
	COMPONENT core-dev
	PARTITION optional_like
	IMPORTS
		std basic tribool
		limits concepts)

eagine_add_module(
	eagine.core.types
	COMPONENT core-dev
	PARTITION holder
	IMPORTS
		std basic tribool
		concepts optional_like)

eagine_add_module(
	eagine.core.types
	COMPONENT core-dev
	PARTITION placeholder
	IMPORTS
		std tribool optional_like)

eagine_add_module(
	eagine.core.types
	COMPONENT core-dev
	PARTITION outcome
	IMPORTS
		std basic limits concepts)

eagine_add_module(
	eagine.core.types
	COMPONENT core-dev
	PARTITION compare
	IMPORTS std basic)

eagine_add_module(
	eagine.core.types
	COMPONENT core-dev
	PARTITION integer_range
	IMPORTS std concepts limits)

eagine_add_module(
	eagine.core.types
	COMPONENT core-dev
	PARTITION enum_map
	IMPORTS std basic bitfield)

eagine_add_module(
	eagine.core.types
	COMPONENT core-dev
	IMPORTS
		eagine.core.build_config)

eagine_add_module_tests(
	eagine.core.types
	UNITS
		anything
		bitfield
		simd_data
		enum_map
		placeholder
		integer_range
		metaprogramming
		function_constant
		nothing
		optional_iterator
		optional_reference
		optional_like_tuple
		valid_if
		tribool
		holder
	IMPORTS
		std)
