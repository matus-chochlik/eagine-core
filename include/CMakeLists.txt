# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
#
configure_file(eagine/config/site.hpp.in eagine/config/site.hpp)
configure_file(eagine/config/build_info.hpp.in eagine/config/build_info.hpp @ONLY)
configure_file(eagine/config/os_info.hpp.in eagine/config/os_info.hpp @ONLY)
configure_file(eagine/config/git_info.hpp.in eagine/config/git_info.hpp @ONLY)
set(GEN_HPPS
	eagine/config/site.hpp
	eagine/config/build_info.hpp 
	eagine/config/os_info.hpp
	eagine/config/git_info.hpp
)
# - testing headers or implementation files ------------------------------------
add_library(eagine-core-testing-headers INTERFACE)

set_property(
	TARGET eagine-core-testing-headers
	APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
# - generated headers or implementation files ----------------------------------
add_library(eagine-core-generated-headers INTERFACE)

set_property(
	TARGET eagine-core-generated-headers
	APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
		${CMAKE_CURRENT_BINARY_DIR})

foreach(FILEPATH ${GEN_HPPS})
	set_property(
		TARGET eagine-core-generated-headers
		APPEND PROPERTY PUBLIC_HEADER ${CMAKE_CURRENT_BINARY_DIR}/${FILEPATH})
endforeach()

foreach(FILEPATH ${GEN_HPPS})
	if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
		install(
			FILES "${CMAKE_CURRENT_BINARY_DIR}/${FILEPATH}"
			DESTINATION share/eagine/debug/include)
	endif()
	if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
		install(
			FILES "${CMAKE_CURRENT_BINARY_DIR}/${FILEPATH}"
			DESTINATION share/eagine/release/include)
	endif()
endforeach()
