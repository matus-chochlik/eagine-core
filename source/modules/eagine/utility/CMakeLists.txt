# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(eagine.core.utility PARTITION integer_hash)
eagine_add_module(eagine.core.utility PARTITION interleaved_call)
eagine_add_module(eagine.core.utility PARTITION protected_member)

eagine_add_module(
	eagine.core.utility PARTITION array_size
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
	eagine.core.utility PARTITION file_magic_number
	IMPORTS
		eagine.core.types
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
		array_size
		bool_aggregate
		double_buffer
		integer_hash
		interleaved_call
		callable_ref
		signal
		memoized
		network_sort
		scope_exit
		sudoku
		cleanup_group
		overloaded
		file_magic_number
		timeline
		timeout
		protected_member
		progress_bar
		random_bytes
		unreachable
	SOURCES
		progress_bar_impl
	IMPORTS
		eagine.core.types
		eagine.core.memory
)
