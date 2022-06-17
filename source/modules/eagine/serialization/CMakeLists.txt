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
	eagine.core.serialization
	INTERFACES
		serialization
	PARTITIONS
		float_utils
		size_and_data
		result
		interface
		read
		write
		implementation
	SOURCES
)

