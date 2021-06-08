# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt

add_subdirectory(lib)

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
	FILE Core.cmake
	NAMESPACE EAGine::
	DESTINATION lib/cmake/EAGine
)
