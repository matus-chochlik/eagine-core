# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.memory PARTITION align
	IMPORTS eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION address
	IMPORTS align eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION bit_density
	IMPORTS eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION null_ptr
	IMPORTS eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION offset_ptr
	IMPORTS
		address
		eagine.core.concepts
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION span
	IMPORTS
		address
		eagine.core.concepts
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION offset_span
	IMPORTS
		span offset_ptr
		eagine.core.concepts
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION string_span
	IMPORTS
		span
		eagine.core.concepts
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION span_algorithm
	IMPORTS span eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION split_span
	IMPORTS
		span span_algorithm
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION aligned_block
	IMPORTS
		span span_algorithm
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION flatten
	IMPORTS
		span
		eagine.core.concepts
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION edit_distance
	IMPORTS span eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION fixed_size_string
	IMPORTS string_span eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION byteset
	IMPORTS span eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION biteset
	IMPORTS byteset eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION byte_allocator
	IMPORTS
		span
		eagine.core.concepts
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION stack_allocator
	IMPORTS
		span byte_allocator
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION std_allocator
	IMPORTS
		span
		byte_allocator
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION buffer
	IMPORTS
		address span
		span_algorithm
		byte_allocator
		eagine.core.build_config
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION object_storage
	IMPORTS
		byte_allocator
		std_allocator
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory
	INTERFACES
		memory
	PARTITIONS
		align
		address
		bit_density
		null_ptr
		offset_ptr
		span
		string_span
		offset_span
		span_algorithm
		split_span
		aligned_block
		byteset
		biteset
		flatten
		edit_distance
		fixed_size_string
		byte_allocator
		stack_allocator
		std_allocator
		buffer
		object_storage
	SOURCES
		memory
		stack_allocator
	IMPORTS
		eagine.core.types
)

eagine_add_module_tests(
	eagine.core.memory
	UNITS
		address
		bit_density
		biteset
		block
		buffer
		byte_allocator
		byteset
		fixed_size_string
		offset_ptr
		offset_span
		span
		span_algorithm
		shared_allocator
		stack_allocator
		std_allocator
	IMPORTS
		eagine.core.concepts
		eagine.core.types
)
