# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.resource
	SOURCES
		resource
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.reflection
		eagine.core.utility
		eagine.core.runtime
		eagine.core.logging
		eagine.core.valid_if
		eagine.core.value_tree
		eagine.core.main_ctx
)

