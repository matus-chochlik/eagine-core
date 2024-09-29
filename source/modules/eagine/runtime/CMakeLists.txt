# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.runtime
	COMPONENT core-dev
	PARTITION signal_switch
	IMPORTS
		std
		eagine.core.utility)

eagine_add_module(
	eagine.core.runtime
	COMPONENT core-dev
	PARTITION process
	IMPORTS
		std
		eagine.core.types)

eagine_add_module(
	eagine.core.runtime
	COMPONENT core-dev
	PARTITION traverse
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.reflection)

eagine_add_module(
	eagine.core.runtime
	COMPONENT core-dev
	PARTITION program_args
	IMPORTS
		std
		eagine.core.debug
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.utility
		eagine.core.valid_if)

eagine_add_module(
	eagine.core.runtime
	COMPONENT core-dev
	PARTITION basic_config
	IMPORTS
		std program_args
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.utility
		eagine.core.valid_if
		eagine.core.reflection)

eagine_add_module(
	eagine.core.runtime
	COMPONENT core-dev
	PARTITION url
	IMPORTS
		std program_args
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.utility
		eagine.core.valid_if
		eagine.core.identifier
		eagine.core.container)

eagine_add_module(
	eagine.core.runtime
	COMPONENT core-dev
	PARTITION compression
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
		eagine.core.utility
		eagine.core.reflection)

eagine_add_module(
	eagine.core.runtime
	COMPONENT core-dev
	PARTITION executable_module
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
		eagine.core.string)

eagine_add_module(
	eagine.core.runtime
	COMPONENT core-dev
	PARTITION file_contents
	IMPORTS
		std input_data
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.utility
		eagine.core.container)

eagine_add_module(
	eagine.core.runtime
	COMPONENT core-dev
	PARTITION value_with_history
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
		eagine.core.valid_if)

eagine_add_module(
	eagine.core.runtime
	COMPONENT core-dev
	PARTITION workshop
	IMPORTS
		std
		eagine.core.types)

eagine_add_module(
	eagine.core.runtime
	COMPONENT core-dev
	SOURCES
		url
		process
		basic_config
		executable_module
		signal_switch
		file_contents
		compression
		workshop
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.utility
		eagine.core.valid_if
		eagine.core.container)

target_link_libraries(
	eagine.core.runtime
	PUBLIC
		EAGine::Deps::System
		EAGine::Deps::SYSTEMD
		EAGine::Deps::ZLIB)

eagine_add_module_tests(
	eagine.core.runtime
	UNITS
		basic_config
		compression
		program_args
		url
	IMPORTS
		std
		eagine.core.debug
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.identifier
		eagine.core.reflection
		eagine.core.units
	ENVIRONMENT
		EAGINE_VAR_MODULE_1=eagine
		EAGINE_VAR_MODULE_2=core
		EAGINE_VAR_MODULE_3=runtime
		EAGINE_VAR_12345=12345
		EAGINE_VAR_FLAG_TRUE=true
		EAGINE_VAR_FLAG_FALSE=false
		EAGINE_VAR_FLAG=)

set_tests_properties(execute-test.eagine.core.runtime.compression PROPERTIES COST 10)
