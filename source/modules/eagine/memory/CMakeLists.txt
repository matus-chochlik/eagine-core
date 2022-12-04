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
	IMPORTS address eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION span
	IMPORTS
		address
		eagine.core.concepts
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION block
	IMPORTS span eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION struct_block
	IMPORTS block
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
	eagine.core.memory PARTITION string_literal
	IMPORTS
		string_span
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION span_io
	IMPORTS span eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION span_algorithm
	IMPORTS span eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION split_span
	IMPORTS
		span block span_algorithm
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION aligned_block
	IMPORTS
		block span_algorithm
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
	IMPORTS block eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION biteset
	IMPORTS byteset eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION byte_allocator
	IMPORTS block eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION stack_allocator
	IMPORTS
		block byte_allocator
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION shared_allocator
	IMPORTS
		byte_allocator
		eagine.core.concepts
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION std_allocator
	IMPORTS
		block
		byte_allocator
		shared_allocator
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION buffer
	IMPORTS
		address block 
		span_algorithm
		shared_allocator
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION buffer_pool
	IMPORTS
		buffer
		shared_allocator
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION alloc_arena
	IMPORTS
		address span block
		byte_allocator
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION data_baking
	IMPORTS
		alloc_arena stack_allocator
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION object_storage
	IMPORTS
		byte_allocator
		shared_allocator
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
		block
		struct_block
		string_span
		string_literal
		offset_span
		span_io
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
		shared_allocator
		std_allocator
		buffer
		buffer_pool
		object_storage
		alloc_arena
		data_baking
	SOURCES
		memory_impl
		stack_allocator_impl
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
		offset_ptr
		offset_span
		span
		span_algorithm
		shared_allocator
		stack_allocator
		std_allocator
)
