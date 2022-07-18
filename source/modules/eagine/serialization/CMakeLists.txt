# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.serialization PARTITION float_utils
	IMPORTS
		eagine.core.types
)

eagine_add_module(
	eagine.core.serialization PARTITION data_buffer
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.reflection
)

eagine_add_module(
	eagine.core.serialization PARTITION size_and_data
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.serialization PARTITION result
	IMPORTS
		eagine.core.types
		eagine.core.reflection
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.serialization PARTITION interface
	IMPORTS
		result
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.reflection
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.serialization PARTITION implementation
	IMPORTS
		result interface
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.reflection
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.serialization PARTITION read
	IMPORTS
		result interface
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
		result interface
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
		result interface
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.serialization PARTITION block
	IMPORTS
		result interface
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.serialization PARTITION packed_block
	IMPORTS
		result block
		eagine.core.types
		eagine.core.memory
		eagine.core.runtime
)

eagine_add_module(
	eagine.core.serialization PARTITION fast_backend
	IMPORTS
		result implementation
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.reflection
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.serialization PARTITION string_backend
	IMPORTS
		result implementation
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
		float_utils result implementation
		eagine.core.concepts
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.identifier
		eagine.core.reflection
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
	INTERFACES
		serialization
	PARTITIONS
		float_utils
		data_buffer
		size_and_data
		result
		interface
		read
		write
		implementation
		iostream
		block
		packed_block
		fast_backend
		string_backend
		portable_backend
		type_sudoku
		type_compiler_info
		type_version_info
	SOURCES
)

