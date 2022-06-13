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
		eagine.core.reflection
)

eagine_add_module(
	eagine.core.value_tree PARTITION wrappers
	IMPORTS
		interface
		eagine.core.types
		eagine.core.memory
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
	eagine.core.value_tree PARTITION app_config
	IMPORTS
		wrappers
		eagine.core.types
		eagine.core.memory
		eagine.core.valid_if
		eagine.core.logging
		eagine.core.runtime
)

eagine_add_module(
	eagine.core.value_tree
	INTERFACES
		value_tree
	PARTITIONS
		interface
		wrappers
		implementation
		#app_config
	SOURCES
		#app_config_impl
)

