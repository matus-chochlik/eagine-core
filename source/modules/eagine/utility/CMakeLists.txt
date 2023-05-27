# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(eagine.core.utility PARTITION count)
eagine_add_module(eagine.core.utility PARTITION coroutine)
eagine_add_module(eagine.core.utility PARTITION overloaded)
eagine_add_module(eagine.core.utility PARTITION double_buffer)
eagine_add_module(eagine.core.utility PARTITION integer_hash)
eagine_add_module(eagine.core.utility PARTITION bool_aggregate)
eagine_add_module(eagine.core.utility PARTITION interleaved_call)
eagine_add_module(eagine.core.utility PARTITION protected_member)
eagine_add_module(eagine.core.utility PARTITION unreachable)

eagine_add_module(
	eagine.core.utility PARTITION sliding_average
	IMPORTS eagine.core.types
)

eagine_add_module(
	eagine.core.utility PARTITION callable_ref
	IMPORTS
		eagine.core.concepts
		eagine.core.types
)

eagine_add_module(
	eagine.core.utility PARTITION signal
	IMPORTS
		callable_ref
		eagine.core.types
		eagine.core.container
)

eagine_add_module(
	eagine.core.utility PARTITION spinlock
	IMPORTS
		eagine.core.build_config
		eagine.core.types
)

eagine_add_module(
	eagine.core.utility PARTITION memoized
	IMPORTS
		callable_ref
		eagine.core.concepts
)

eagine_add_module(
	eagine.core.utility PARTITION network_sort
	IMPORTS
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.utility PARTITION scope_exit
	IMPORTS callable_ref eagine.core.types
)

eagine_add_module(
	eagine.core.utility PARTITION sudoku
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.container
)

eagine_add_module(
	eagine.core.utility PARTITION cleanup_group
	IMPORTS
		scope_exit
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.utility PARTITION timeline
	IMPORTS
		eagine.core.types
)

eagine_add_module(
	eagine.core.utility PARTITION timeout
	IMPORTS
		eagine.core.types
		eagine.core.debug
)

eagine_add_module(
	eagine.core.utility PARTITION random_bytes
	IMPORTS
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.utility PARTITION progress_bar
	IMPORTS
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.utility
	INTERFACES
		utility
	PARTITIONS
		count
		bool_aggregate
		sliding_average
		coroutine
		double_buffer
		integer_hash
		interleaved_call
		callable_ref
		signal
		spinlock
		memoized
		network_sort
		scope_exit
		sudoku
		cleanup_group
		overloaded
		timeline
		timeout
		protected_member
		progress_bar
		random_bytes
		unreachable
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
		eagine.core.concepts
		eagine.core.types
		eagine.core.memory
		eagine.core.debug
		eagine.core.container
		eagine.core.valid_if
)

set_tests_properties(execute-test.eagine.core.utility.sudoku PROPERTIES COST 30)
set_tests_properties(execute-test.eagine.core.utility.timeout PROPERTIES COST 20)
