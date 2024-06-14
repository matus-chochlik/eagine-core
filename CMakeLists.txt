# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt
#
cmake_minimum_required(VERSION 3.27)

project(
	EAGineCore
	LANGUAGES CXX
	HOMEPAGE_URL https://github.com/matus-chochlik/eagine-core)

if(NOT EAGINE_CORE_ROOT)
	set(EAGINE_CORE_ROOT ${PROJECT_SOURCE_DIR})
endif()
if(NOT EAGINE_CORE_BINARY_ROOT)
	set(EAGINE_CORE_BINARY_ROOT ${PROJECT_BINARY_DIR})
endif()
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/config)
file(STRINGS VERSION EAGINE_VERSION)

include(EAGineCoreCommonBasic)
include(EAGineCppFeature)

find_package(Threads REQUIRED)
find_package(EAGineCommonLibs)
find_package(EAGineSystemd)
find_package(EAGineZLIB)
find_package(EAGineBoostSpirit)
find_package(EAGineStacktrace)

include(EAGineCoreCommonFinish)
include(EAGineOSInfo)

add_subdirectory(submodules)
enable_testing()
add_subdirectory(config)
add_subdirectory(include)
add_subdirectory(source)
add_subdirectory(example)
add_subdirectory(deploy)
add_subdirectory(docker)

