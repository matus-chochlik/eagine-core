# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.math PARTITION sign
)

eagine_add_module(
	eagine.core.math PARTITION traits
	IMPORTS
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.math PARTITION constants
	IMPORTS eagine.core.types
)

eagine_add_module(
	eagine.core.math PARTITION functions
	IMPORTS constants
)

eagine_add_module(
	eagine.core.math PARTITION scalar
	IMPORTS eagine.core.vectorization
)

eagine_add_module(
	eagine.core.math PARTITION vector
	IMPORTS
		traits scalar
		eagine.core.concepts
		eagine.core.types
		eagine.core.memory
		eagine.core.vectorization
)

eagine_add_module(
	eagine.core.math PARTITION matrix
	IMPORTS
		traits vector
		eagine.core.concepts
		eagine.core.types
		eagine.core.vectorization
)

eagine_add_module(
	eagine.core.math PARTITION matrix_construct
	IMPORTS
		traits vector matrix
		eagine.core.types
		eagine.core.memory
		eagine.core.vectorization
		eagine.core.units
)

eagine_add_module(
	eagine.core.math PARTITION tmat
	IMPORTS
		traits vector
		matrix matrix_construct
		eagine.core.concepts
		eagine.core.types
		eagine.core.memory
		eagine.core.vectorization
)

eagine_add_module(
	eagine.core.math PARTITION colors
	IMPORTS
		constants functions vector
		eagine.core.types
)

eagine_add_module(
	eagine.core.math PARTITION primitives
	IMPORTS
		vector
		eagine.core.types
)

eagine_add_module(
	eagine.core.math PARTITION curves
	IMPORTS
		functions
		eagine.core.types
		eagine.core.memory
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.math PARTITION intersection
	IMPORTS
		vector
		functions
		primitives
		eagine.core.types
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.math PARTITION coordinates
	IMPORTS
		functions vector
		eagine.core.valid_if
		eagine.core.units
)

eagine_add_module(
	eagine.core.math PARTITION animated_value
	IMPORTS
		functions
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.math PARTITION io
	IMPORTS
		primitives
		vector
		matrix
)

eagine_add_module(
	eagine.core.math
	INTERFACES
		math
	PARTITIONS
		sign
		traits
		constants
		functions
		scalar
		vector
		matrix
		matrix_construct
		tmat
		colors
		curves
		primitives
		intersection
		coordinates
		animated_value
		io
	IMPORTS
		eagine.core.types
		eagine.core.vectorization
)

eagine_add_module_tests(
	eagine.core.math
	UNITS
		coordinates
		functions
		scalar
		vector
		matrix_1
		matrix_2
		matrix_3
		matrix_construct
	IMPORTS
		eagine.core.concepts
		eagine.core.debug
		eagine.core.types
		eagine.core.memory
		eagine.core.vectorization
		eagine.core.valid_if
		eagine.core.units
)
