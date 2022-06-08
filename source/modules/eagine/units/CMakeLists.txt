# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
#eagine_add_module(eagine.core.types PARTITION basic)
add_subdirectory(framework)

#eagine_add_module(
#eagine.core.units PARTITION basic
#IMPORTS
#eagine.coretypes.
#eagine.core.units.framework
#)

eagine_add_module(
	eagine.core.units
	INTERFACES units
	PARTITIONS
	#basic
	IMPORTS
		eagine.core.units.framework
)

