# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.build_info FRAGMENT os
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.valid_if
)
target_link_libraries(
	eagine.core.build_info.os
	PUBLIC eagine-core-generated-headers
)

eagine_add_module(
	eagine.core.build_info FRAGMENT git
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.valid_if
)
target_link_libraries(
	eagine.core.build_info.git
	PUBLIC eagine-core-generated-headers
)

eagine_add_module(
	eagine.core.build_info FRAGMENT version
	IMPORTS
		git
		eagine.core.types
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.build_info
	INTERFACES
		build_info
	FRAGMENTS
		os
		git
		version
	SOURCES
		os_impl
		git_impl
)

