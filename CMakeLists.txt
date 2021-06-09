# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
#
cmake_minimum_required(VERSION 3.10)

project(EAGineCore)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/config)
file(STRINGS VERSION EAGINE_VERSION)

include(EAGineCoreCommon)
include(EAGineCppFeature)

find_package(Threads REQUIRED)
find_package(EAGineCommonLibs)
find_package(EAGineSystemd)
find_package(EAGineZLIB)
find_package(EAGineBoostSpirit)
find_package(EAGineStacktrace)

include(EAGineGitInfo)

add_subdirectory(submodules)

add_subdirectory(config)
add_subdirectory(include)
add_subdirectory(source)
add_subdirectory(example)

# tests
if(NOT ${NO_TESTS})
	add_subdirectory(test)
endif()

