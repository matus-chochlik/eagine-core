# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt

add_subdirectory(lib)

add_library(EAGine::Core INTERFACE IMPORTED GLOBAL)
target_link_libraries(EAGine::Core INTERFACE eagine-core)

install(
	TARGETS eagine-core
	EXPORT Core
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)

install(
	EXPORT Core
	NAMESPACE EAGine::
	FILE Core.cmake
	DESTINATION lib/cmake/EAGine
)
