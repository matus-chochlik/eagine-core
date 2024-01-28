# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.serialization PARTITION float_utils
	IMPORTS
		std
		eagine.core.types
)

eagine_add_module(
	eagine.core.serialization PARTITION data_buffer
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.reflection
)

eagine_add_module(
	eagine.core.serialization PARTITION size_and_data
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.serialization PARTITION result
	IMPORTS
		std
		eagine.core.types
		eagine.core.reflection
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.serialization PARTITION interface
	IMPORTS
		std result
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.reflection
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.serialization PARTITION implementation
	IMPORTS
		std result interface
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.reflection
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.serialization PARTITION read
	IMPORTS
		std result interface
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.reflection
		eagine.core.math
		eagine.core.valid_if
		eagine.core.units
)

eagine_add_module(
	eagine.core.serialization PARTITION write
	IMPORTS
		std result interface
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.reflection
		eagine.core.math
		eagine.core.valid_if
		eagine.core.units
)

eagine_add_module(
	eagine.core.serialization PARTITION iostream
	IMPORTS
		std result interface
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.serialization PARTITION block
	IMPORTS
		std result interface
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.serialization PARTITION packed_block
	IMPORTS
		std result block
		eagine.core.types
		eagine.core.memory
		eagine.core.runtime
)

eagine_add_module(
	eagine.core.serialization PARTITION fast_backend
	IMPORTS
		std result implementation
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.reflection
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.serialization PARTITION string_backend
	IMPORTS
		std result implementation
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.identifier
		eagine.core.reflection
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.serialization PARTITION portable_backend
	IMPORTS
		std float_utils result implementation
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.identifier
		eagine.core.reflection
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.serialization PARTITION valtree_backend
	IMPORTS
		std result interface
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.identifier
		eagine.core.reflection
		eagine.core.value_tree
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.serialization PARTITION type_sudoku
	IMPORTS
		data_buffer read write
		eagine.core.types
		eagine.core.memory
		eagine.core.utility
)

eagine_add_module(
	eagine.core.serialization PARTITION type_compiler_info
	IMPORTS
		data_buffer read write
		eagine.core.types
		eagine.core.memory
		eagine.core.build_info
)

eagine_add_module(
	eagine.core.serialization PARTITION type_version_info
	IMPORTS
		data_buffer read write
		eagine.core.types
		eagine.core.memory
		eagine.core.build_info
)

eagine_add_module(
	eagine.core.serialization
)

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
		eagine.core.units
)
