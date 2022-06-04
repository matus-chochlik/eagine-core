# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.utility FRAGMENT bit_density
	IMPORTS eagine.core.types
)

eagine_add_module(
	eagine.core.utility FRAGMENT optional_ref
	IMPORTS eagine.core.types
)

eagine_add_module(
	eagine.core.utility FRAGMENT callable_ref
	IMPORTS
		eagine.core.concepts
		eagine.core.types
)

eagine_add_module(
	eagine.core.utility FRAGMENT scope_exit
	IMPORTS callable_ref eagine.core.types
)

eagine_add_module(
	eagine.core.utility FRAGMENT timeout
	IMPORTS
		eagine.core.types
		eagine.core.debug
)

eagine_add_module(
	eagine.core.utility FRAGMENT program_args
	IMPORTS
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.utility
	INTERFACES
		utility
	FRAGMENTS
		count
		bool_aggregate
		double_buffer
		iterator
		integer_hash
		integer_range
		bit_density
		optional_ref
		callable_ref
		scope_exit
		overloaded
		timeout
		signal_switch
		program_args
)

