# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(eagine.core.utility PARTITION integer_hash)
eagine_add_module(eagine.core.utility PARTITION protected_member)

eagine_add_module(
	eagine.core.utility PARTITION callable_ref
	IMPORTS
		eagine.core.concepts
		eagine.core.types
)

eagine_add_module(
	eagine.core.utility PARTITION memoized
	IMPORTS callable_ref
)

eagine_add_module(
	eagine.core.utility PARTITION scope_exit
	IMPORTS callable_ref eagine.core.types
)

eagine_add_module(
	eagine.core.utility PARTITION timeout
	IMPORTS
		eagine.core.types
		eagine.core.debug
)

eagine_add_module(
	eagine.core.utility PARTITION progress_bar
	IMPORTS
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.utility PARTITION file_contents
	IMPORTS
		protected_member
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
		double_buffer
		integer_hash
		callable_ref
		memoized
		scope_exit
		overloaded
		timeout
		protected_member
		progress_bar
		file_contents
)
