# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.value_tree PARTITION interface
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.identifier
		eagine.core.reflection
)

eagine_add_module(
	eagine.core.value_tree PARTITION wrappers
	IMPORTS
		interface
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.reflection
)

eagine_add_module(
	eagine.core.value_tree PARTITION implementation
	IMPORTS
		interface wrappers
		eagine.core.types
		eagine.core.memory
		eagine.core.logging
)

eagine_add_module(
	eagine.core.value_tree PARTITION overlay
	IMPORTS
		interface wrappers
		eagine.core.types
		eagine.core.logging
)


eagine_add_module(
	eagine.core.value_tree
	INTERFACES
		value_tree
	PARTITIONS
		interface
		wrappers
		implementation
		#overlay
	SOURCES
		empty_impl
		json_impl
		yaml_impl
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.identifier
		eagine.core.logging
	PRIVATE_INCLUDE_DIRECTORIES
		"${PROJECT_SOURCE_DIR}/submodules/rapidjson/include"
	PRIVATE_LINK_LIBRARIES
		ryml
)

