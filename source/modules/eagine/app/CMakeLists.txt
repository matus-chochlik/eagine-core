# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.app
	COMPONENT core-dev
	PARTITION special_args
	IMPORTS
		std
		eagine.core.identifier
		eagine.core.main_ctx
		eagine.core.resource
)

eagine_add_module(
	eagine.core.app
	COMPONENT core-dev
	SOURCES
		special_args
	IMPORTS
		std
		eagine.core.build_info
		eagine.core.types
		eagine.core.identifier
		eagine.core.memory
		eagine.core.main_ctx
		eagine.core.resource
)

