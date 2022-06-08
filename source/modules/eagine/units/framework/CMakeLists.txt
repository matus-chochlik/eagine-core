# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.units.framework PARTITION base
	IMPORTS eagine.core.types
)

eagine_add_module(
	eagine.core.units.framework PARTITION unit
	IMPORTS base eagine.core.types
)

eagine_add_module(
	eagine.core.units.framework PARTITION quantity
	IMPORTS base eagine.core.types
)

eagine_add_module(
	eagine.core.units.framework
	INTERFACES framework
	PARTITIONS
		base
		unit
		quantity
	IMPORTS
		eagine.core.types
)

