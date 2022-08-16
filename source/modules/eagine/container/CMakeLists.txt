# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(eagine.core.container PARTITION iterator)

eagine_add_module(
	eagine.core.container PARTITION flat_set
	IMPORTS eagine.core.types;
)
eagine_add_module(
	eagine.core.container PARTITION flat_map
	IMPORTS eagine.core.types;
)
eagine_add_module(
	eagine.core.container PARTITION wrapping
	IMPORTS
		iterator
		eagine.core.types;
		eagine.core.memory;
		eagine.core.valid_if;
)

eagine_add_module(
	eagine.core.container
	INTERFACES container
	PARTITIONS
		iterator
		flat_set
		flat_map
		wrapping
)

