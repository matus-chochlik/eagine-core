# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.console
	COMPONENT core-dev
	PARTITION backend
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.reflection
		eagine.core.runtime
		eagine.core.utility
)

eagine_add_module(
	eagine.core.console
	COMPONENT core-dev
	PARTITION entry
	IMPORTS
		std backend
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.identifier
		eagine.core.valid_if
		eagine.core.runtime
		eagine.core.logging
)

eagine_add_module(
	eagine.core.console
	COMPONENT core-dev
	SOURCES
		console
		null_backend
		iostream_backend
	IMPORTS
		std
		eagine.core.types
		eagine.core.identifier
		eagine.core.container
		eagine.core.memory
		eagine.core.string
		eagine.core.utility
		eagine.core.runtime
)

