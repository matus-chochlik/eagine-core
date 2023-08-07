# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.memory PARTITION align
	IMPORTS std eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION address
	IMPORTS std align eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION bit_density
	IMPORTS std eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION null_ptr
	IMPORTS std eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION offset_ptr
	IMPORTS
		std address
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION span
	IMPORTS
		std address
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION offset_span
	IMPORTS
		std span offset_ptr
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION string_span
	IMPORTS
		std span
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION span_algorithm
	IMPORTS std span eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION split_span
	IMPORTS
		std span span_algorithm
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION aligned_block
	IMPORTS
		std span span_algorithm
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION flatten
	IMPORTS
		std span
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION edit_distance
	IMPORTS std span eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION fixed_size_string
	IMPORTS std string_span eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION byteset
	IMPORTS std span eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION biteset
	IMPORTS std byteset eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION byte_allocator
	IMPORTS
		std span
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION stack_allocator
	IMPORTS
		std span byte_allocator
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION std_allocator
	IMPORTS
		std span
		byte_allocator
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION buffer
	IMPORTS
		std address span
		span_algorithm
		byte_allocator
		eagine.core.build_config
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory PARTITION object_storage
	IMPORTS
		std byte_allocator
		std_allocator
		eagine.core.types
)

eagine_add_module(
	eagine.core.memory
	SOURCES
		memory
		stack_allocator
	IMPORTS
		std
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
		std
		eagine.core.types
)
