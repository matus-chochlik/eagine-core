# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.container PARTITION iterator
	IMPORTS std
)

eagine_add_module(
	eagine.core.container PARTITION static_vector
	IMPORTS
		std
		eagine.core.types
)
eagine_add_module(
	eagine.core.container PARTITION small_vector
	IMPORTS
		std static_vector
		eagine.core.types
)
eagine_add_module(
	eagine.core.container PARTITION chunk_list
	IMPORTS
		std static_vector
		eagine.core.types
		eagine.core.memory
)
eagine_add_module(
	eagine.core.container PARTITION object_pool
	IMPORTS
		std small_vector
		eagine.core.types
)
eagine_add_module(
	eagine.core.container PARTITION flat_set
	IMPORTS
		std static_vector chunk_list
		eagine.core.types
		eagine.core.memory
)
eagine_add_module(
	eagine.core.container PARTITION flat_map
	IMPORTS
		std static_vector chunk_list
		eagine.core.types
		eagine.core.memory
)
eagine_add_module(
	eagine.core.container PARTITION trie
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
)
eagine_add_module(
	eagine.core.container PARTITION wrapping
	IMPORTS
		std iterator
		eagine.core.types
		eagine.core.memory
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.container
	IMPORTS
		std
		eagine.core.types
)

eagine_add_module_tests(
	eagine.core.container
	UNITS
		iterator
		flat_map
		flat_set
		chunk_list
		object_pool
		trie
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
		eagine.core.valid_if
)
