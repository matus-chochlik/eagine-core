#  Copyright Matus Chochlik.
#  Distributed under the Boost Software License, Version 1.0.
#  See accompanying file LICENSE_1_0.txt or copy at
#   http://www.boost.org/LICENSE_1_0.txt
#
find_package(ZLIB)

add_library(EAGine::Core::Deps::ZLIB INTERFACE IMPORTED)
if(ZLIB_FOUND)
	if(ZLIB_INCLUDE_DIRS)
		target_include_directories(
			EAGine::Core::Deps::ZLIB INTERFACE "${ZLIB_INCLUDE_DIRS}"
		)
	endif()

	if(ZLIB_LIBRARY_DIRS)
		set_target_properties(
			EAGine::Core::Deps::ZLIB PROPERTIES
			INTERFACE_LINK_DIRECTORIES "${ZLIB_LIBRARY_DIRS}"
		)
	endif()

	target_compile_definitions(
		EAGine::Core::Deps::ZLIB INTERFACE EAGINE_USE_ZLIB=1
	)

	target_link_libraries(
		EAGine::Core::Deps::ZLIB INTERFACE "${ZLIB_LIBRARIES}"
	)
endif()


