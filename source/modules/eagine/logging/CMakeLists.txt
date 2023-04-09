# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.logging PARTITION entry_arg
	IMPORTS
		eagine.core.build_info
		eagine.core.types
		eagine.core.reflection
		eagine.core.identifier
		eagine.core.valid_if
		eagine.core.runtime
		eagine.core.utility
		eagine.core.string
		eagine.core.c_api
)

eagine_add_module(
	eagine.core.logging PARTITION backend
	IMPORTS
		entry_arg
		eagine.core.types
		eagine.core.memory
		eagine.core.runtime
		eagine.core.identifier
)

eagine_add_module(
	eagine.core.logging PARTITION config
	IMPORTS
		backend
		eagine.core.build_config
		eagine.core.types
)

eagine_add_module(
	eagine.core.logging PARTITION fast_xml_backend
	IMPORTS
		backend
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
		backend fast_xml_backend
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.identifier
		eagine.core.reflection
		eagine.core.utility
)

eagine_add_module(
	eagine.core.logging PARTITION time_interval
	IMPORTS
		backend
		eagine.core.types
		eagine.core.identifier
)

eagine_add_module(
	eagine.core.logging PARTITION entry
	IMPORTS
		entry_arg backend
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.logging PARTITION logger
	IMPORTS
		config backend
		entry time_interval
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
		entry backend logger
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
		entry_arg
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.c_api
)

eagine_add_module(
	eagine.core.logging
	INTERFACES
		logging
	PARTITIONS
		config
		entry_arg
		backend
		fast_xml_backend
		ostream_backend
		time_interval
		entry
		logger
		root_logger
		adapt_entry_arg
	SOURCES
		null_backend
		asio_backend
		syslog_backend
		proxy_backend
		root_logger
	IMPORTS
		eagine.core.debug
		eagine.core.memory
	PRIVATE_INCLUDE_DIRECTORIES
		"${PROJECT_SOURCE_DIR}/submodules/asio/asio/include"
)


