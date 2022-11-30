# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.vectorization PARTITION data_array
	IMPORTS eagine.core.concepts
)

eagine_add_module(
	eagine.core.vectorization PARTITION data_simd
	IMPORTS eagine.core.concepts
)

eagine_add_module(
	eagine.core.vectorization PARTITION data
	IMPORTS data_array data_simd
)

eagine_add_module(
	eagine.core.vectorization PARTITION shuffle
	IMPORTS
		data_vector data
		eagine.core.types
)

eagine_add_module(
	eagine.core.vectorization PARTITION from
	IMPORTS
		data
		eagine.core.types
)

eagine_add_module(
	eagine.core.vectorization PARTITION hsum
	IMPORTS
		data shuffle
		eagine.core.types
)

eagine_add_module(
	eagine.core.vectorization PARTITION esum
	IMPORTS
		data hsum
		eagine.core.types
)

eagine_add_module(
	eagine.core.vectorization PARTITION sdiv
	IMPORTS
		data
		eagine.core.types
)

eagine_add_module(
	eagine.core.vectorization PARTITION abs
	IMPORTS data
)

eagine_add_module(
	eagine.core.vectorization PARTITION sqrt
	IMPORTS data
)

eagine_add_module(
	eagine.core.vectorization PARTITION axis
	IMPORTS data
)

eagine_add_module(
	eagine.core.vectorization PARTITION fill
	IMPORTS data
)

eagine_add_module(
	eagine.core.vectorization PARTITION cast
	IMPORTS data fill
)

eagine_add_module(
	eagine.core.vectorization PARTITION compare
	IMPORTS data
)

eagine_add_module(
	eagine.core.vectorization PARTITION diff
	IMPORTS data abs
)

eagine_add_module(
	eagine.core.vectorization PARTITION view
	IMPORTS
		data
		eagine.core.memory
)

eagine_add_module(
	eagine.core.vectorization
	INTERFACES
		vectorization
	PARTITIONS
		data_array
		data_simd
		data
		from
		shuffle
		abs
		sqrt
		axis
		cast
		fill
		compare
		diff
		hsum
		esum
		sdiv
		view
)

eagine_add_module_tests(
	eagine.core.vectorization
	UNITS
		data
)
