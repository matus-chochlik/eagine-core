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
include(./Sources.cmake)
# - regular headers ------------------------------------------------------------
add_library(eagine-core-headers INTERFACE)

target_include_directories(
	eagine-core-headers
	INTERFACE
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

foreach(FILENAME ${HEADERS} ${PUB_INLS})
	set_property(
		TARGET eagine-core-headers
		APPEND PROPERTY PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}
	)
endforeach()
# - private headers or implementation files ------------------------------------
add_library(eagine-core-private-headers INTERFACE)

target_include_directories(
	eagine-core-private-headers
	INTERFACE
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
		"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/submodules/asio/asio/include>"
		"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/submodules/rapidjson/include>"
)

foreach(FILENAME ${LIB_INLS})
	set_property(
		TARGET eagine-core-private-headers
		APPEND PROPERTY PRIVATE_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}
	)
endforeach()
# - generated headers or implementation files ----------------------------------
add_library(eagine-core-generated-headers INTERFACE)

target_include_directories(
	eagine-core-generated-headers
	INTERFACE
	$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
)

foreach(FILENAME ${GEN_HPPS})
	set_property(
		TARGET eagine-core-generated-headers
		APPEND PROPERTY PUBLIC_HEADER ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}
	)
endforeach()

