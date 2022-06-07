# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
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
		scalar
		eagine.core.concepts
		eagine.core.types
		eagine.core.vectorization
)

eagine_add_module(
	eagine.core.math PARTITION animated_value
	IMPORTS
		functions
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.math
	INTERFACES
		math
	PARTITIONS
		constants
		functions
		scalar
		vector
		animated_value
)


