# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.memory FRAGMENT align
	IMPORTS eagine.core.types
)

eagine_add_module(
	eagine.core.memory FRAGMENT address
	IMPORTS align eagine.core.types
)

eagine_add_module(
	eagine.core.memory FRAGMENT null_ptr
	IMPORTS eagine.core.types
)

eagine_add_module(
	eagine.core.memory FRAGMENT offset_ptr
	IMPORTS address eagine.core.types
)

eagine_add_module(
	eagine.core.memory FRAGMENT span
	IMPORTS address eagine.core.types
)

eagine_add_module(
	eagine.core.memory FRAGMENT block
	IMPORTS span eagine.core.types
)

eagine_add_module(
	eagine.core.memory FRAGMENT string_span
	IMPORTS
		span
		eagine.core.concepts
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory FRAGMENT span_io
	IMPORTS span eagine.core.types
)

eagine_add_module(
	eagine.core.memory FRAGMENT span_algorithm
	IMPORTS span eagine.core.types
)

eagine_add_module(
	eagine.core.memory FRAGMENT edit_distance
	IMPORTS span eagine.core.types
)

eagine_add_module(
	eagine.core.memory FRAGMENT fixed_size_string
	IMPORTS string_span eagine.core.types
)

eagine_add_module(
	eagine.core.memory FRAGMENT byteset
	IMPORTS block eagine.core.types
)

eagine_add_module(
	eagine.core.memory FRAGMENT biteset
	IMPORTS byteset eagine.core.types
)

eagine_add_module(
	eagine.core.memory FRAGMENT byte_allocator
	IMPORTS block eagine.core.types
)

eagine_add_module(
	eagine.core.memory FRAGMENT shared_allocator
	IMPORTS
		byte_allocator
		eagine.core.concepts
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory
	INTERFACES
		memory
	FRAGMENTS
		align
		address
		null_ptr
		offset_ptr
		span
		block
		string_span
		span_io
		span_algorithm
		byteset
		biteset
		fixed_size_string
		edit_distance
		byte_allocator
		shared_allocator
)

