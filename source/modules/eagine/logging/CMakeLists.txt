# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.logging PARTITION entry_arg
	IMPORTS
		std
		eagine.core.types
		eagine.core.reflection
		eagine.core.identifier
)

eagine_add_module(
	eagine.core.logging PARTITION backend
	IMPORTS
		std entry_arg
		eagine.core.types
		eagine.core.memory
		eagine.core.runtime
		eagine.core.identifier
		eagine.core.reflection
)

eagine_add_module(
	eagine.core.logging PARTITION config
	IMPORTS
		std backend
		eagine.core.build_config
		eagine.core.types
)

eagine_add_module(
	eagine.core.logging PARTITION fast_json_backend
	IMPORTS
		std backend
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.identifier
		eagine.core.reflection
		eagine.core.utility
)

eagine_add_module(
	eagine.core.logging PARTITION fast_xml_backend
	IMPORTS
		std backend
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.identifier
		eagine.core.reflection
		eagine.core.utility
)

eagine_add_module(
	eagine.core.logging PARTITION ostream_backend
	IMPORTS
		std backend
		fast_xml_backend
		fast_json_backend
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.identifier
		eagine.core.reflection
		eagine.core.utility
)

eagine_add_module(
	eagine.core.logging PARTITION entry
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
	eagine.core.logging PARTITION logger
	IMPORTS
		std config backend entry
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.valid_if
		eagine.core.runtime
		eagine.core.units
)

eagine_add_module(
	eagine.core.logging PARTITION root_logger
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
	eagine.core.logging PARTITION adapt_entry_arg
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
	SOURCES
		null_backend
		asio_backend
		syslog_backend
		proxy_backend
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


