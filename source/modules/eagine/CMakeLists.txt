# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
add_subdirectory(build_config)
add_subdirectory(concepts)
add_subdirectory(types)
add_subdirectory(debug)
add_subdirectory(memory)
add_subdirectory(identifier)
add_subdirectory(utility)
add_subdirectory(valid_if)
add_subdirectory(vectorization)
add_subdirectory(reflection)
add_subdirectory(build_info)
add_subdirectory(units)
add_subdirectory(math)
add_subdirectory(container)
add_subdirectory(string)
add_subdirectory(runtime)
add_subdirectory(logging)


eagine_add_module(
	eagine.core
	PP_NAME CORE
	INTERFACES core
	IMPORTS
		eagine.core.build_config
		eagine.core.concepts
		eagine.core.types
		eagine.core.debug
		eagine.core.memory
		eagine.core.identifier
		eagine.core.utility
		eagine.core.valid_if
		eagine.core.vectorization
		eagine.core.reflection
		eagine.core.build_info
		eagine.core.units
		eagine.core.math
		eagine.core.container
		eagine.core.string
		eagine.core.runtime
		eagine.core.logging
)

