# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.build_info PARTITION os
	IMPORTS
		eagine.core.memory
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.build_info PARTITION compiler
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.build_info PARTITION git
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.build_info PARTITION version
	IMPORTS
		git compiler
		eagine.core.types
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.build_info
	INTERFACES
		build_info
	PARTITIONS
		os
		git
		compiler
		version
	SOURCES
		os_impl
		git_impl
		compiler_impl
	PRIVATE_LINK_LIBRARIES
		eagine-core-generated-headers
)

