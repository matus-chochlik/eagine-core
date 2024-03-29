#  Copyright Matus Chochlik.
#  Distributed under the Boost Software License, Version 1.0.
#  See accompanying file LICENSE_1_0.txt or copy at
#  https://www.boost.org/LICENSE_1_0.txt
#
find_package(ZLIB)

add_library(EAGine::Deps::ZLIB INTERFACE IMPORTED)
if(ZLIB_FOUND)
	if(ZLIB_INCLUDE_DIRS)
		target_include_directories(
			EAGine::Deps::ZLIB INTERFACE "${ZLIB_INCLUDE_DIRS}"
		)
	endif()

	if(ZLIB_LIBRARY_DIRS)
		set_target_properties(
			EAGine::Deps::ZLIB PROPERTIES
			INTERFACE_LINK_DIRECTORIES "${ZLIB_LIBRARY_DIRS}"
		)
	endif()

	target_link_libraries(
		EAGine::Deps::ZLIB INTERFACE "${ZLIB_LIBRARIES}"
	)
	set(EAGINE_USE_ZLIB 1)
	set(EAGINE_USE_ZLIB 1 PARENT_SCOPE)
else()
	set(EAGINE_USE_ZLIB 0)
	set(EAGINE_USE_ZLIB 0 PARENT_SCOPE)
endif()
