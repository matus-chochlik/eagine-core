# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(eagine.core.container PARTITION iterator)

eagine_add_module(
	eagine.core.container PARTITION static_vector
)
eagine_add_module(
	eagine.core.container PARTITION small_vector
	IMPORTS static_vector
)
eagine_add_module(
	eagine.core.container PARTITION flat_set
	IMPORTS eagine.core.types;
)
eagine_add_module(
	eagine.core.container PARTITION flat_map
	IMPORTS eagine.core.types;
)
eagine_add_module(
	eagine.core.container PARTITION trie
	IMPORTS
		eagine.core.types;
		eagine.core.memory;
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
		static_vector
		small_vector
		flat_set
		flat_map
		trie
		wrapping
)

eagine_add_module_tests(
	eagine.core.container
	UNITS
		iterator
		flat_map
		flat_set
		trie
)
