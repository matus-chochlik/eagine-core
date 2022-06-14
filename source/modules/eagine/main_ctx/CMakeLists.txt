# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.main_ctx PARTITION interface
	IMPORTS
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
	eagine.core.main_ctx PARTITION system_info
	IMPORTS
		interface
		eagine.core.types
		eagine.core.memory
		eagine.core.valid_if
		eagine.core.reflection
		eagine.core.units
)

eagine_add_module(
	eagine.core.main_ctx PARTITION user_info
	IMPORTS
		interface
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.valid_if
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
		eagine.core.valid_if
		eagine.core.runtime
		eagine.core.logging
		eagine.core.value_tree
)


#eagine_add_module(
#	eagine.core.main_ctx PARTITION storage
#	IMPORTS
#		interface parent object
#		system_info user_info watchdog
#		eagine.core.types
#		eagine.core.build_info
#		eagine.core.types
#		eagine.core.memory
#		eagine.core.runtime
#		eagine.core.logging
#		eagine.core.console
#		eagine.core.progress
#)

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
	SOURCES
		system_info_impl
		user_info_impl
		watchdog_impl
		app_config_impl
)

