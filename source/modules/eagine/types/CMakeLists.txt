# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(eagine.core.types FRAGMENT basic)
eagine_add_module(eagine.core.types FRAGMENT interface)

eagine_add_module(
	eagine.core.types FRAGMENT bitfield
	IMPORTS eagine.core.concepts
)

eagine_add_module(
	eagine.core.types FRAGMENT extract
	IMPORTS eagine.core.concepts
)

eagine_add_module(
	eagine.core.types FRAGMENT compare
	IMPORTS basic
)

eagine_add_module(
	eagine.core.types FRAGMENT limits
	IMPORTS basic
)

eagine_add_module(
	eagine.core.types FRAGMENT integer
	IMPORTS limits
)

eagine_add_module(
	eagine.core.types FRAGMENT selector
	IMPORTS basic
)

eagine_add_module(
	eagine.core.types FRAGMENT enum_map
	IMPORTS bitfield selector
)

eagine_add_module(
	eagine.core.types
	INTERFACES types
	FRAGMENTS
		basic
		nothing
		anything
		interface
		compare
		limits
		integer
		extract
		tribool
		bitfield
		function_constant
		selector
		enum_map
)

