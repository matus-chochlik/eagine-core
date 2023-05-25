# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(eagine.core.runtime PARTITION signal_switch)

eagine_add_module(
	eagine.core.runtime PARTITION process
	IMPORTS
		eagine.core.types
)

eagine_add_module(
	eagine.core.runtime PARTITION traverse
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.reflection
)

eagine_add_module(
	eagine.core.runtime PARTITION program_args
	IMPORTS
		eagine.core.debug
		eagine.core.concepts
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.runtime PARTITION basic_config
	IMPORTS
		program_args
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.utility
		eagine.core.valid_if
		eagine.core.reflection
)

eagine_add_module(
	eagine.core.runtime PARTITION url
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.valid_if
		eagine.core.identifier
		eagine.core.container
)

eagine_add_module(
	eagine.core.runtime PARTITION compression
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.utility
		eagine.core.reflection
)

eagine_add_module(
	eagine.core.runtime PARTITION input_data
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.utility
)

eagine_add_module(
	eagine.core.runtime PARTITION executable_module
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.string
)

eagine_add_module(
	eagine.core.runtime PARTITION file_contents
	IMPORTS
		input_data
		eagine.core.types
		eagine.core.memory
		eagine.core.utility
)

eagine_add_module(
	eagine.core.runtime PARTITION value_with_history
	IMPORTS
		eagine.core.types
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.runtime PARTITION workshop
	IMPORTS
		eagine.core.types
)

eagine_add_module(
	eagine.core.runtime
	INTERFACES
		runtime
	PARTITIONS
		signal_switch
		traverse	
		basic_config
		process
		program_args
		url
		input_data
		executable_module
		file_contents
		compression
		value_with_history
		workshop
	SOURCES
		url
		process
		input_data
		basic_config
		executable_module
		signal_switch
		file_contents
		compression
		workshop
	IMPORTS
		eagine.core.types
		eagine.core.utility
		eagine.core.memory
		eagine.core.string
		eagine.core.valid_if
)
target_link_libraries(
	eagine.core.runtime
	PUBLIC
		EAGine::Deps::System
		EAGine::Deps::SYSTEMD
		EAGine::Deps::ZLIB
)

eagine_add_module_tests(
	eagine.core.runtime
	UNITS
		basic_config
		compression
		program_args
		url
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.string
	ENVIRONMENT
		EAGINE_VAR_MODULE_1=eagine
		EAGINE_VAR_MODULE_2=core
		EAGINE_VAR_MODULE_3=runtime
		EAGINE_VAR_12345=12345
		EAGINE_VAR_FLAG_TRUE=true
		EAGINE_VAR_FLAG_FALSE=false
		EAGINE_VAR_FLAG=
)
set_tests_properties(execute-test.eagine.core.runtime.compression PROPERTIES COST 10)
