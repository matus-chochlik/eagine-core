# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/asio/asio")
	execute_process(
		COMMAND git submodule update --init --recursive
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/asio")
endif()

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/CMakeLists.txt")
	execute_process(
		COMMAND git submodule update --init --recursive
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/rapidjson")
endif()

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/rapidyaml/CMakeLists.txt")
	execute_process(
		COMMAND git submodule update --init --recursive
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/rapidyaml")
endif()

add_subdirectory(rapidyaml ryml EXCLUDE_FROM_ALL)

install(
	TARGETS ryml
	COMPONENT core-dev
	DESTINATION lib)
