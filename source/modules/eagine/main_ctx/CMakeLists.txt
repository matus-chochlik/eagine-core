# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.main_ctx PARTITION interface
	IMPORTS
		std
		eagine.core.build_info
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.utility
		eagine.core.runtime
		eagine.core.logging
		eagine.core.progress
		eagine.core.console
)

eagine_add_module(
	eagine.core.main_ctx PARTITION parent
	IMPORTS
		std interface
		eagine.core.runtime
)

eagine_add_module(
	eagine.core.main_ctx PARTITION object
	IMPORTS
		std parent
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.utility
		eagine.core.runtime
		eagine.core.logging
		eagine.core.console
		eagine.core.progress
)

eagine_add_module(
	eagine.core.main_ctx PARTITION system_info
	IMPORTS
		std parent object
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.valid_if
		eagine.core.reflection
		eagine.core.units
)

eagine_add_module(
	eagine.core.main_ctx PARTITION user_info
	IMPORTS
		std parent object
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.main_ctx PARTITION watchdog
	IMPORTS
		std interface parent object
		eagine.core.types
		eagine.core.utility
		eagine.core.identifier
)

eagine_add_module(
	eagine.core.main_ctx PARTITION app_config
	IMPORTS
		std interface
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.container
		eagine.core.identifier
		eagine.core.reflection
		eagine.core.valid_if
		eagine.core.runtime
		eagine.core.logging
		eagine.core.value_tree
)

eagine_add_module(
	eagine.core.main_ctx PARTITION storage
	IMPORTS
		std interface parent object app_config
		system_info user_info watchdog
		eagine.core.build_info
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.container
		eagine.core.valid_if
		eagine.core.utility
		eagine.core.runtime
		eagine.core.logging
		eagine.core.console
		eagine.core.progress
)

eagine_add_module(
	eagine.core.main_ctx PARTITION main
	IMPORTS
		std interface
)

eagine_add_module(
	eagine.core.main_ctx
	SOURCES
		system_info
		user_info
		watchdog
		app_config
		main_ctx
		object
		main
	IMPORTS
		std
		eagine.core.build_config
		eagine.core.build_info
		eagine.core.types
		eagine.core.debug
		eagine.core.memory
		eagine.core.string
		eagine.core.identifier
		eagine.core.serialization
		eagine.core.units
		eagine.core.utility
		eagine.core.runtime
		eagine.core.logging
		eagine.core.console
		eagine.core.progress
		eagine.core.valid_if
		eagine.core.value_tree
	INTERFACE_LINK_LIBRARIES
		systemd
)

eagine_add_module_tests(
	eagine.core.main_ctx
	UNITS
		system_info
		user_info
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
		eagine.core.c_api
)
set_tests_properties(execute-test.eagine.core.main_ctx.system_info PROPERTIES COST 10)
