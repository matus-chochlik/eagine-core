# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.build_config
	COMPONENT core-dev
	PRIVATE_INCLUDE_DIRECTORIES
		"${EAGINE_CORE_BINARY_ROOT}/include"
	PRIVATE_LINK_LIBRARIES
		eagine-core-generated-headers)

