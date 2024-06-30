# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.utility
	COMPONENT core-dev
	PARTITION count
	IMPORTS std eagine.core.types
)
eagine_add_module(
	eagine.core.utility
	COMPONENT core-dev
	PARTITION coroutine
	IMPORTS std
)
eagine_add_module(
	eagine.core.utility
	COMPONENT core-dev
	PARTITION overloaded
	IMPORTS std
)
eagine_add_module(
	eagine.core.utility
	COMPONENT core-dev
	PARTITION double_buffer
	IMPORTS std
)
eagine_add_module(
	eagine.core.utility
	COMPONENT core-dev
	PARTITION interleaved_call
	IMPORTS std
)
eagine_add_module(
	eagine.core.utility
	COMPONENT core-dev
	PARTITION integer_hash
	IMPORTS std
)
eagine_add_module(
	eagine.core.utility
	COMPONENT core-dev
	PARTITION bool_aggregate
	IMPORTS std
)
eagine_add_module(
	eagine.core.utility
	COMPONENT core-dev
	PARTITION protected_member
	IMPORTS std
)
eagine_add_module(
	eagine.core.utility
	COMPONENT core-dev
	PARTITION unreachable
	IMPORTS std
)

eagine_add_module(
	eagine.core.utility
	COMPONENT core-dev
	PARTITION sliding_average
	IMPORTS std eagine.core.types
)

eagine_add_module(
	eagine.core.utility
	COMPONENT core-dev
	PARTITION callable_ref
	IMPORTS
		std eagine.core.types
)

eagine_add_module(
	eagine.core.utility
	COMPONENT core-dev
	PARTITION signal
	IMPORTS
		std callable_ref
		eagine.core.types
		eagine.core.container
)

eagine_add_module(
	eagine.core.utility
	COMPONENT core-dev
	PARTITION spinlock
	IMPORTS
		std
		eagine.core.build_config
		eagine.core.types
)

eagine_add_module(
	eagine.core.utility
	COMPONENT core-dev
	PARTITION memoized
	IMPORTS
		std callable_ref
		eagine.core.types
		eagine.core.container
)

eagine_add_module(
	eagine.core.utility
	COMPONENT core-dev
	PARTITION network_sort
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.utility
	COMPONENT core-dev
	PARTITION scope_exit
	IMPORTS std callable_ref eagine.core.types
)

eagine_add_module(
	eagine.core.utility
	COMPONENT core-dev
	PARTITION sudoku
	IMPORTS
		std generator
		eagine.core.types
		eagine.core.memory
		eagine.core.container
)

eagine_add_module(
	eagine.core.utility
	COMPONENT core-dev
	PARTITION cleanup_group
	IMPORTS
		std scope_exit
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.utility
	COMPONENT core-dev
	PARTITION timeline
	IMPORTS
		std
		eagine.core.types
)

eagine_add_module(
	eagine.core.utility
	COMPONENT core-dev
	PARTITION timeout
	IMPORTS
		std
		eagine.core.types
		eagine.core.debug
)

eagine_add_module(
	eagine.core.utility
	COMPONENT core-dev
	PARTITION scheduler
	IMPORTS
		std
		eagine.core.types
		eagine.core.identifier
		eagine.core.container
)

eagine_add_module(
	eagine.core.utility
	COMPONENT core-dev
	PARTITION random_bytes
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.utility
	COMPONENT core-dev
	PARTITION progress_bar
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.utility
	COMPONENT core-dev
	SOURCES
		random_bytes
		progress_bar
		scheduler
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.container
		eagine.core.identifier
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
		scheduler
		timeout
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
		eagine.core.debug
		eagine.core.identifier
		eagine.core.container
		eagine.core.valid_if
)

set_tests_properties(execute-test.eagine.core.utility.sudoku PROPERTIES COST 30)
set_tests_properties(execute-test.eagine.core.utility.timeout PROPERTIES COST 20)
