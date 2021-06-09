# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
#
cmake_minimum_required(VERSION 3.10)

include(config/Policies.cmake)

project(EAGineCore)
file(STRINGS VERSION EAGINE_VERSION)

include(config/Defaults.cmake)
include(config/BuildType.cmake)
include(config/StaticAnalysis.cmake)
include(config/Compiler.cmake)
include(config/CppFeature.cmake)
include(config/LowProfile.cmake)

include(config/CommonFindMod.cmake)
include(config/CommonLibs.cmake)

include(config/FindThreads.cmake)
include(config/FindStacktrace.cmake)
include(config/FindBoostSpirit.cmake)
include(config/FindSystemd.cmake)
include(config/FindZLIB.cmake)

# now create the site-configuration headers
include(config/GitInfo.cmake)

add_subdirectory(submodules)

add_subdirectory(include)
add_subdirectory(source)
add_subdirectory(example)

# tests
if(NOT ${NO_TESTS})
	add_subdirectory(test)
endif()

