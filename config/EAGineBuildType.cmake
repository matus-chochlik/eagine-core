#  Copyright Matus Chochlik.
#  Distributed under the Boost Software License, Version 1.0.
#  See accompanying file LICENSE_1_0.txt or copy at
#  https://www.boost.org/LICENSE_1_0.txt
#
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "No build type selected, default to Release")
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (default Release)")
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
	set(EAGINE_DEBUG 0)
else()
	set(EAGINE_DEBUG 1)
endif()
