# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.main_ctx PARTITION parent
	IMPORTS
		eagine.core.runtime
)

eagine_add_module(
	eagine.core.main_ctx PARTITION object
	IMPORTS
		parent
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.utility
		eagine.core.runtime
		eagine.core.logging
		eagine.core.console
		eagine.core.progress
)

eagine_add_module(
	eagine.core.main_ctx
	INTERFACES
		main_ctx
	PARTITIONS
		parent
		object
	SOURCES
)

