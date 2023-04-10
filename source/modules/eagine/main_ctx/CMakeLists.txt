# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.main_ctx PARTITION system_info
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.valid_if
		eagine.core.reflection
		eagine.core.units
)

eagine_add_module(
	eagine.core.main_ctx PARTITION user_info
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.main_ctx PARTITION interface
	IMPORTS
		system_info user_info
		eagine.core.build_info
		eagine.core.types
		eagine.core.memory
		eagine.core.utility
		eagine.core.runtime
		eagine.core.logging
		eagine.core.progress
		eagine.core.console
)

eagine_add_module(
	eagine.core.main_ctx PARTITION parent
	IMPORTS
		interface
		eagine.core.runtime
)

eagine_add_module(
	eagine.core.main_ctx PARTITION object
	IMPORTS
		parent
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
	eagine.core.main_ctx PARTITION watchdog
	IMPORTS
		interface parent object
		eagine.core.types
		eagine.core.identifier
)

eagine_add_module(
	eagine.core.main_ctx PARTITION app_config
	IMPORTS
		interface
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.container
		eagine.core.valid_if
		eagine.core.runtime
		eagine.core.logging
		eagine.core.value_tree
)

eagine_add_module(
	eagine.core.main_ctx PARTITION storage
	IMPORTS
		interface parent object app_config
		system_info user_info watchdog
		eagine.core.build_info
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.container
		eagine.core.runtime
		eagine.core.logging
		eagine.core.console
		eagine.core.progress
)

eagine_add_module(
	eagine.core.main_ctx PARTITION main
	IMPORTS
		interface
)

eagine_add_module(
	eagine.core.main_ctx
	INTERFACES
		main_ctx
	PARTITIONS
		interface
		parent
		object
		system_info
		user_info
		watchdog
		app_config
		storage
		main
	SOURCES
		system_info
		user_info
		watchdog
		app_config
		main_ctx
		main
	IMPORTS
		eagine.core.build_config
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.units
		eagine.core.runtime
		eagine.core.logging
		eagine.core.console
		eagine.core.progress
		eagine.core.value_tree
)

eagine_add_module_tests(
	eagine.core.main_ctx
	UNITS
		system_info
		user_info
	IMPORTS
		eagine.core.types
		eagine.core.memory
)
set_tests_properties(execute-test.eagine.core.main_ctx.system_info PROPERTIES COST 10)
