# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.logging PARTITION severity
	IMPORTS eagine.core.reflection
)

eagine_add_module(
	eagine.core.logging PARTITION config
	IMPORTS severity eagine.core.types
)

eagine_add_module(
	eagine.core.logging PARTITION entry_arg
	IMPORTS
		eagine.core.types
		eagine.core.reflection
		eagine.core.identifier
)

eagine_add_module(
	eagine.core.logging PARTITION backend
	IMPORTS
		entry_arg
		severity
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
)

eagine_add_module(
	eagine.core.logging PARTITION null_backend
	IMPORTS
		backend
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
)

eagine_add_module(
	eagine.core.logging PARTITION ostream_backend
	IMPORTS
		backend
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.string
		eagine.core.reflection
)

eagine_add_module(
	eagine.core.logging PARTITION syslog_backend
	IMPORTS
		backend
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.string
		eagine.core.container
)

eagine_add_module(
	eagine.core.logging PARTITION proxy_backend
	IMPORTS
		backend
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.string
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
		config entry backend
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.valid_if
		eagine.core.units.framework
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
)

eagine_add_module(
	eagine.core.logging
	INTERFACES
		logging
	PARTITIONS
		severity
		config
		entry_arg
		backend
		null_backend
		ostream_backend
		syslog_backend
		proxy_backend
		entry
		logger
		root_logger
	SOURCES
		null_backend_impl
		proxy_backend_impl
		root_logger_impl
)


