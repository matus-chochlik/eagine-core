# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt

include(EAGineCXXModules)

add_subdirectory(eagine)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/mod_test.cpp")
	add_executable(mod_test mod_test.cpp)
	eagine_target_module(mod_test eagine.core)
endif()

