#  Copyright Matus Chochlik.
#  Distributed under the Boost Software License, Version 1.0.
#  See accompanying file LICENSE_1_0.txt or copy at
#   http://www.boost.org/LICENSE_1_0.txt
#
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(WITH_REFLECTION)
	set(CMAKE_CXX_FLAGS -freflection)
	get_filename_component(EAGINE_COMPILER_BIN_DIR "${CMAKE_CXX_COMPILER}" PATH)
	get_filename_component(EAGINE_COMPILER_PREFIX "${EAGINE_COMPILER_BIN_DIR}" PATH)
	include_directories("${EAGINE_COMPILER_PREFIX}/include")
	set(EAGINE_CXX_REFLECTION 0)
else()
	set(CMAKE_CXX_EXTENSIONS OFF)
	set(EAGINE_CXX_REFLECTION 0)
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
