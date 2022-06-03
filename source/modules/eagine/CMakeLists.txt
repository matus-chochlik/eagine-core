# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
add_subdirectory(concepts)
add_subdirectory(types)
add_subdirectory(debug)
add_subdirectory(memory)
add_subdirectory(utility)
add_subdirectory(valid_if)
add_subdirectory(identifier)

add_subdirectory(math)

eagine_add_module(
	eagine.core
	INTERFACES core
	IMPORTS
		eagine.core.concepts
		eagine.core.types
		eagine.core.debug
		eagine.core.memory
		eagine.core.utility
		eagine.core.valid_if
		eagine.core.identifier
		eagine.core.math
)

