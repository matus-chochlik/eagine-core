#  Copyright Matus Chochlik.
#  Distributed under the Boost Software License, Version 1.0.
#  See accompanying file LICENSE_1_0.txt or copy at
#   http://www.boost.org/LICENSE_1_0.txt
#
if(WITH_REFLECTION)
	set(CMAKE_CXX_STANDARD 20)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
	set(CMAKE_CXX_FLAGS -freflection)
	add_definitions(-DEAGINE_CXX_REFLECTION=1)
	get_filename_component(EAGINE_COMPILER_BIN_DIR "${CMAKE_CXX_COMPILER}" PATH)
	get_filename_component(EAGINE_COMPILER_PREFIX "${EAGINE_COMPILER_BIN_DIR}" PATH)
	include_directories("${EAGINE_COMPILER_PREFIX}/include")
else()
	set(CMAKE_CXX_STANDARD 17)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
	set(CMAKE_CXX_EXTENSIONS OFF)
endif()

include(EAGinePolicies)
include(EAGineDefaults)
include(EAGineBuildType)
include(EAGineStaticAnalysis)
include(EAGineCompiler)
include(EAGineLowProfile)
include(EAGineCommonFindMod)
include(EAGineBaking)
include(EAGineEmbed)
include(EAGinePaths)
