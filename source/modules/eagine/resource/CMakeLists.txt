# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.resource
	PP_NAME CORE_RESOURCE
	INTERFACES
		resource
	SOURCES
		resource
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
		eagine.core.reflection
		eagine.core.utility
		eagine.core.runtime
		eagine.core.logging
		eagine.core.value_tree
		eagine.core.main_ctx
)

eagine_add_module_tests(
	eagine.core.resource
	UNITS
		resource
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.runtime
)

eagine_embed_target_resources(
	TARGET test.eagine.core.resource.resource
	RESOURCES
		CMakeLists "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt"
	ENABLE_SEARCH
)

