# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.resource
	COMPONENT core-dev
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
		eagine.core.main_ctx)

