# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.progress
	COMPONENT core-dev
	PARTITION backend
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
		eagine.core.utility
		eagine.core.runtime
		eagine.core.logging
)

eagine_add_module(
	eagine.core.progress
	COMPONENT core-dev
	PARTITION activity
	IMPORTS
		std backend
		eagine.core.types
		eagine.core.memory
		eagine.core.utility
		eagine.core.runtime
)

eagine_add_module(
	eagine.core.progress
	COMPONENT core-dev
	PARTITION root_activity
	IMPORTS
		std backend activity
		eagine.core.types
		eagine.core.memory
		eagine.core.utility
		eagine.core.logging
)

eagine_add_module(
	eagine.core.progress
	COMPONENT core-dev
	SOURCES
		default_backend
		activity
		root_activity
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.container
		eagine.core.utility
		eagine.core.runtime
		eagine.core.valid_if
		eagine.core.logging
)

