# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.console PARTITION entry_kind
	IMPORTS
		eagine.core.types
		eagine.core.reflection
)

eagine_add_module(
	eagine.core.console PARTITION backend
	IMPORTS
		entry_kind
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.runtime
		eagine.core.utility
)

eagine_add_module(
	eagine.core.console PARTITION null_backend
	IMPORTS
		entry_kind backend
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.identifier
)

eagine_add_module(
	eagine.core.console PARTITION iostream_backend
	IMPORTS
		entry_kind backend
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.identifier
		eagine.core.container
		eagine.core.utility
)

eagine_add_module(
	eagine.core.console PARTITION entry
	IMPORTS
		entry_kind backend
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.valid_if
		eagine.core.logging
)

eagine_add_module(
	eagine.core.console
	INTERFACES
		console
	PARTITIONS
		entry_kind
		entry
		backend
		null_backend
		iostream_backend
	SOURCES
		console_impl
		null_backend_impl
		iostream_backend_impl
	IMPORTS
		eagine.core.types
		eagine.core.identifier
		eagine.core.memory
		eagine.core.runtime
)

