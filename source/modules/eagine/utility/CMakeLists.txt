# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(eagine.core.utility PARTITION count)
eagine_add_module(
	eagine.core.utility PARTITION coroutine
	IMPORTS std
)
eagine_add_module(
	eagine.core.utility PARTITION overloaded
	IMPORTS std
)
eagine_add_module(
	eagine.core.utility PARTITION double_buffer
	IMPORTS std
)
eagine_add_module(
	eagine.core.utility PARTITION interleaved_call
	IMPORTS std
)
eagine_add_module(
	eagine.core.utility PARTITION integer_hash
	IMPORTS std
)
eagine_add_module(
	eagine.core.utility PARTITION bool_aggregate
	IMPORTS std
)
eagine_add_module(
	eagine.core.utility PARTITION protected_member
	IMPORTS std
)
eagine_add_module(
	eagine.core.utility PARTITION unreachable
	IMPORTS std
)

eagine_add_module(
	eagine.core.utility PARTITION sliding_average
	IMPORTS std eagine.core.types
)

eagine_add_module(
	eagine.core.utility PARTITION callable_ref
	IMPORTS
		std eagine.core.types
)

eagine_add_module(
	eagine.core.utility PARTITION signal
	IMPORTS
		std callable_ref
		eagine.core.types
		eagine.core.container
)

eagine_add_module(
	eagine.core.utility PARTITION spinlock
	IMPORTS
		std
		eagine.core.build_config
		eagine.core.types
)

eagine_add_module(
	eagine.core.utility PARTITION memoized
	IMPORTS
		std callable_ref
		eagine.core.types
)

eagine_add_module(
	eagine.core.utility PARTITION network_sort
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.utility PARTITION scope_exit
	IMPORTS std callable_ref eagine.core.types
)

eagine_add_module(
	eagine.core.utility PARTITION sudoku
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
		eagine.core.container
)

eagine_add_module(
	eagine.core.utility PARTITION cleanup_group
	IMPORTS
		std scope_exit
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.utility PARTITION timeline
	IMPORTS
		std
		eagine.core.types
)

eagine_add_module(
	eagine.core.utility PARTITION timeout
	IMPORTS
		std
		eagine.core.types
		eagine.core.debug
)

eagine_add_module(
	eagine.core.utility PARTITION random_bytes
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.utility PARTITION progress_bar
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.utility
	SOURCES
		progress_bar
	IMPORTS
		eagine.core.types
		eagine.core.memory
)

eagine_add_module_tests(
	eagine.core.utility
	UNITS
		bool_aggregate
		callable_ref
		double_buffer
		memoized
		network_sort
		overloaded
		protected_member
		random_bytes
		scope_exit
		sudoku
		timeout
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.debug
		eagine.core.container
		eagine.core.valid_if
)

set_tests_properties(execute-test.eagine.core.utility.sudoku PROPERTIES COST 30)
set_tests_properties(execute-test.eagine.core.utility.timeout PROPERTIES COST 20)
