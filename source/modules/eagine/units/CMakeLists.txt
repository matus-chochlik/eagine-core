# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt
#eagine_add_module(eagine.core.types PARTITION basic)
eagine_add_module(
	eagine.core.units PARTITION base
	IMPORTS std eagine.core.types
)

eagine_add_module(
	eagine.core.units PARTITION dimension
	IMPORTS base eagine.core.types
)

eagine_add_module(
	eagine.core.units PARTITION dimensions
	IMPORTS base dimension eagine.core.types
)

eagine_add_module(
	eagine.core.units PARTITION unit
	IMPORTS std base eagine.core.types
)

eagine_add_module(
	eagine.core.units PARTITION si
	IMPORTS
		std base unit dimension dimensions
		eagine.core.types
)

eagine_add_module(
	eagine.core.units PARTITION quantity
	IMPORTS std base eagine.core.types
)

eagine_add_module(
	eagine.core.units PARTITION strings
	IMPORTS
		std base unit dimension
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.units
	IMPORTS
		std
		eagine.core.types
		eagine.core.units.framework
)

eagine_add_module_tests(
	eagine.core.units
	UNITS
		base
		dimensions
		unit
		quantity
		si
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
)
