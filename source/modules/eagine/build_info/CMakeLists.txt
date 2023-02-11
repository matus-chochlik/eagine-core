# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.build_info
	INTERFACES
		build_info
	SOURCES
		os
		build_info
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.valid_if
	PRIVATE_LINK_LIBRARIES
		eagine-core-generated-headers
)

