# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.logging
	COMPONENT core-dev
	PARTITION entry_arg
	IMPORTS
		std
		eagine.core.types
		eagine.core.reflection
		eagine.core.identifier
)

eagine_add_module(
	eagine.core.logging
	COMPONENT core-dev
	PARTITION backend
	IMPORTS
		std entry_arg
		eagine.core.types
		eagine.core.memory
		eagine.core.runtime
		eagine.core.identifier
		eagine.core.reflection
)

eagine_add_module(
	eagine.core.logging
	COMPONENT core-dev
	PARTITION config
	IMPORTS
		std backend
		eagine.core.build_config
		eagine.core.types
)

eagine_add_module(
	eagine.core.logging
	COMPONENT core-dev
	PARTITION entry
	IMPORTS
		std entry_arg backend
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.utility
		eagine.core.runtime
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.logging
	COMPONENT core-dev
	PARTITION logger
	IMPORTS
		std config backend entry
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.valid_if
		eagine.core.utility
		eagine.core.runtime
		eagine.core.units
)

eagine_add_module(
	eagine.core.logging
	COMPONENT core-dev
	PARTITION root_logger
	IMPORTS
		std entry backend logger
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.build_info
		eagine.core.valid_if
		eagine.core.utility
		eagine.core.runtime
)

eagine_add_module(
	eagine.core.logging
	COMPONENT core-dev
	PARTITION adapt_entry_arg
	IMPORTS
		std entry_arg
		eagine.core.build_info
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.identifier
		eagine.core.runtime
		eagine.core.valid_if
		eagine.core.c_api
)

eagine_add_module(
	eagine.core.logging
	COMPONENT core-dev
	SOURCES
		backend
		null_backend
		syslog_backend
		json_backend
		xml_backend
		proxy_backend
		std_output_stream
		asio_output_stream
		factory
		logger
		root_logger
	IMPORTS
		std
		eagine.core.build_config
		eagine.core.types
		eagine.core.build_info
		eagine.core.debug
		eagine.core.memory
		eagine.core.string
		eagine.core.identifier
		eagine.core.container
		eagine.core.reflection
		eagine.core.utility
		eagine.core.valid_if
		eagine.core.runtime
	PRIVATE_INCLUDE_DIRECTORIES
		"${PROJECT_SOURCE_DIR}/submodules/asio/asio/include"
)

eagine_add_module_tests(
	eagine.core.logging
	UNITS
		backend
	IMPORTS
		std
		eagine.core.debug
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.utility
		eagine.core.identifier
		eagine.core.valid_if
		eagine.core.units
		eagine.core.c_api
)
