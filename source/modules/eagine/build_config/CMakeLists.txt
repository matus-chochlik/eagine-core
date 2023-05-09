# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.build_config
	INTERFACES
		build_config
	PRIVATE_INCLUDE_DIRECTORIES
		"${PROJECT_BINARY_DIR}/include"
	PRIVATE_LINK_LIBRARIES
		eagine-core-generated-headers
)

