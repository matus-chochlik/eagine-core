# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.resource PARTITION embedded
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.reflection
		eagine.core.utility
		eagine.core.runtime
		eagine.core.main_ctx
)

eagine_add_module(
	eagine.core.resource PARTITION embed
	IMPORTS
		embedded
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.runtime
)

eagine_add_module(
	eagine.core.resource PARTITION fetch
	IMPORTS
		embedded embed
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.valid_if
		eagine.core.runtime
		eagine.core.logging
		eagine.core.main_ctx
)

eagine_add_module(
	eagine.core.resource
	PP_NAME CORE_RESOURCE
	INTERFACES
		resource
	PARTITIONS
		embedded
		embed
		fetch
	SOURCES
		embed_impl
		embedded_impl
	IMPORTS
)

