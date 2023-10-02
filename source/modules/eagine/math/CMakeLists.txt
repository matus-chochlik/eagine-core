# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.math PARTITION sign
	IMPORTS std
)

eagine_add_module(
	eagine.core.math PARTITION traits
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.math PARTITION constants
	IMPORTS
		std
		eagine.core.types
)

eagine_add_module(
	eagine.core.math PARTITION functions
	IMPORTS
		std constants
)

eagine_add_module(
	eagine.core.math PARTITION scalar
	IMPORTS
		std
		eagine.core.vectorization
)

eagine_add_module(
	eagine.core.math PARTITION vector
	IMPORTS
		std traits scalar
		eagine.core.types
		eagine.core.memory
		eagine.core.vectorization
)

eagine_add_module(
	eagine.core.math PARTITION matrix
	IMPORTS
		std traits vector
		eagine.core.types
		eagine.core.vectorization
)

eagine_add_module(
	eagine.core.math PARTITION matrix_construct
	IMPORTS
		std traits vector matrix
		eagine.core.types
		eagine.core.memory
		eagine.core.vectorization
		eagine.core.units
)

eagine_add_module(
	eagine.core.math PARTITION tmat
	IMPORTS
		std traits vector
		matrix matrix_construct
		eagine.core.types
		eagine.core.memory
		eagine.core.vectorization
)

eagine_add_module(
	eagine.core.math PARTITION colors
	IMPORTS
		std constants functions vector
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.math PARTITION primitives
	IMPORTS
		vector
		std eagine.core.types
)

eagine_add_module(
	eagine.core.math PARTITION curves
	IMPORTS
		std functions
		eagine.core.types
		eagine.core.memory
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.math PARTITION intersection
	IMPORTS
		std vector
		functions primitives
		eagine.core.types
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.math PARTITION coordinates
	IMPORTS
		std functions vector
		eagine.core.valid_if
		eagine.core.units
)

eagine_add_module(
	eagine.core.math PARTITION animated_value
	IMPORTS
		std functions
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.math PARTITION io
	IMPORTS
		std
		primitives
		vector
		matrix
)

eagine_add_module(
	eagine.core.math
	SOURCES
		colors
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.container
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
		eagine.core.debug
		eagine.core.types
		eagine.core.memory
		eagine.core.vectorization
		eagine.core.valid_if
		eagine.core.units
)
