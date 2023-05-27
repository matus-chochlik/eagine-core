# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.vectorization PARTITION data_array
	IMPORTS eagine.core.types
)

eagine_add_module(
	eagine.core.vectorization PARTITION data_simd
	IMPORTS eagine.core.types
)

eagine_add_module(
	eagine.core.vectorization PARTITION data
	IMPORTS data_array data_simd
)

eagine_add_module(
	eagine.core.vectorization PARTITION ops
	IMPORTS
		data
		data_simd
		eagine.core.types
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
		ops
)

eagine_add_module_tests(
	eagine.core.vectorization
	UNITS
		abs
		axis
		compare
		data
		diff
		fill
		from
		hsum
		shuffle
		view
	IMPORTS
		eagine.core.types
		eagine.core.memory
)
