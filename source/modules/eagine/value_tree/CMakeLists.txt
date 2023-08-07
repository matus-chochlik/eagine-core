# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.value_tree PARTITION interface
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.identifier
		eagine.core.reflection
		eagine.core.valid_if
		eagine.core.console
)

eagine_add_module(
	eagine.core.value_tree PARTITION wrappers
	IMPORTS
		std interface
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.identifier
		eagine.core.utility
		eagine.core.reflection
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.value_tree PARTITION implementation
	IMPORTS
		std interface wrappers
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.utility
		eagine.core.runtime
		eagine.core.logging
)

eagine_add_module(
	eagine.core.value_tree
	SOURCES
		wrappers
		empty
		json
		yaml
		filesystem
		overlay
		visitors
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.identifier
		eagine.core.utility
		eagine.core.valid_if
		eagine.core.logging
		eagine.core.console
	PRIVATE_INCLUDE_DIRECTORIES
		"${PROJECT_SOURCE_DIR}/submodules/rapidjson/include"
	PRIVATE_LINK_LIBRARIES
		ryml
	INTERFACE_LINK_LIBRARIES
		ryml
)

eagine_add_module_tests(
	eagine.core.value_tree
	UNITS
		wrappers
		visitors
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.reflection
		eagine.core.logging
		eagine.core.units
		eagine.core.c_api
)

set_tests_properties(execute-test.eagine.core.value_tree.visitors PROPERTIES COST 3)
