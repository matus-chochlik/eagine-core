# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.progress PARTITION backend
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.utility
)

eagine_add_module(
	eagine.core.progress PARTITION default_backend
	IMPORTS
		backend
		eagine.core.types
		eagine.core.memory
		eagine.core.utility
		eagine.core.identifier
		eagine.core.container
		eagine.core.runtime
		eagine.core.logging
)

eagine_add_module(
	eagine.core.progress PARTITION activity
	IMPORTS
		backend
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.progress PARTITION root_activity
	IMPORTS
		backend activity
		eagine.core.types
		eagine.core.memory
		eagine.core.logging
)

eagine_add_module(
	eagine.core.progress
	INTERFACES
		progress
	PARTITIONS
		backend
		default_backend
		activity
		root_activity
	SOURCES
		default_backend_impl
		root_activity_impl
)

