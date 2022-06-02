# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.utils FRAGMENT optional_ref
	IMPORTS eagine.core.types
)

eagine_add_module(
	eagine.core.utils FRAGMENT callable_ref
	IMPORTS eagine.core.types
)

eagine_add_module(
	eagine.core.utils FRAGMENT scope_exit
	IMPORTS callable_ref eagine.core.types
)

eagine_add_module(
	eagine.core.utils FRAGMENT timeout
	IMPORTS
		eagine.core.types
		eagine.core.debug
)

eagine_add_module(
	eagine.core.utils
	INTERFACES
		utils
	FRAGMENTS
		count
		bool_aggregate
		double_buffer
		interface
		iterator
		integer_hash
		integer_range
		optional_ref
		callable_ref
		scope_exit
		overloaded
		timeout
)

