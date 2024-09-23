# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.serialization
	COMPONENT core-dev
	PARTITION float_utils
	IMPORTS
		std
		eagine.core.types)

eagine_add_module(
	eagine.core.serialization
	COMPONENT core-dev
	PARTITION data_buffer
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.reflection)

eagine_add_module(
	eagine.core.serialization
	COMPONENT core-dev
	PARTITION size_and_data
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.valid_if)

eagine_add_module(
	eagine.core.serialization
	COMPONENT core-dev
	PARTITION result
	IMPORTS
		std
		eagine.core.types
		eagine.core.reflection
		eagine.core.valid_if)

eagine_add_module(
	eagine.core.serialization
	COMPONENT core-dev
	PARTITION interface
	IMPORTS
		std result
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.reflection
		eagine.core.valid_if)

eagine_add_module(
	eagine.core.serialization
	COMPONENT core-dev
	PARTITION implementation
	IMPORTS
		std result interface
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.reflection
		eagine.core.valid_if)

eagine_add_module(
	eagine.core.serialization
	COMPONENT core-dev
	PARTITION read
	IMPORTS
		std result interface
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.identifier
		eagine.core.reflection
		eagine.core.math
		eagine.core.valid_if
		eagine.core.units)

eagine_add_module(
	eagine.core.serialization
	COMPONENT core-dev
	PARTITION write
	IMPORTS
		std result interface
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.reflection
		eagine.core.math
		eagine.core.valid_if
		eagine.core.units)

eagine_add_module(
	eagine.core.serialization
	COMPONENT core-dev
	PARTITION iostream
	IMPORTS
		std result interface
		eagine.core.types
		eagine.core.memory)

eagine_add_module(
	eagine.core.serialization
	COMPONENT core-dev
	PARTITION block
	IMPORTS
		std result interface
		eagine.core.types
		eagine.core.memory)

eagine_add_module(
	eagine.core.serialization
	COMPONENT core-dev
	PARTITION packed_block
	IMPORTS
		std result block
		eagine.core.types
		eagine.core.memory
		eagine.core.runtime)

eagine_add_module(
	eagine.core.serialization
	COMPONENT core-dev
	PARTITION fast_backend
	IMPORTS
		std result implementation
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.reflection
		eagine.core.valid_if)

eagine_add_module(
	eagine.core.serialization
	COMPONENT core-dev
	PARTITION string_backend
	IMPORTS
		std result implementation
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.identifier
		eagine.core.reflection
		eagine.core.valid_if)

eagine_add_module(
	eagine.core.serialization
	COMPONENT core-dev
	PARTITION portable_backend
	IMPORTS
		std float_utils result implementation
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.identifier
		eagine.core.reflection
		eagine.core.valid_if)

eagine_add_module(
	eagine.core.serialization
	COMPONENT core-dev
	PARTITION valtree_backend
	IMPORTS
		std result interface
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.identifier
		eagine.core.reflection
		eagine.core.value_tree
		eagine.core.valid_if)

eagine_add_module(
	eagine.core.serialization
	COMPONENT core-dev
	PARTITION type_sudoku
	IMPORTS
		data_buffer read write
		eagine.core.types
		eagine.core.memory
		eagine.core.utility)

eagine_add_module(
	eagine.core.serialization
	COMPONENT core-dev
	PARTITION type_compiler_info
	IMPORTS
		data_buffer read write
		eagine.core.types
		eagine.core.memory
		eagine.core.build_info)

eagine_add_module(
	eagine.core.serialization
	COMPONENT core-dev
	PARTITION type_version_info
	IMPORTS
		data_buffer read write
		eagine.core.types
		eagine.core.memory
		eagine.core.build_info)

eagine_add_module(
	eagine.core.serialization
	COMPONENT core-dev)

eagine_add_module_tests(
	eagine.core.serialization
	UNITS
		float_utils
		size_and_data
	IMPORTS
		std
		eagine.core.types
		eagine.core.debug
		eagine.core.memory
		eagine.core.string
		eagine.core.identifier
		eagine.core.build_info
		eagine.core.utility
		eagine.core.valid_if
		eagine.core.value_tree
		eagine.core.c_api
		eagine.core.units)
