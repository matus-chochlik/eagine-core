# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt

function(eagine_add_module_common_properties TARGET_NAME)
	set_property(
		TARGET ${TARGET_NAME}
		APPEND PROPERTY COMPILE_OPTIONS
		"-fmodules-ts"
	)
	set_property(
		TARGET ${TARGET_NAME}
		APPEND PROPERTY PRIVATE_COMPILE_OPTIONS
		"-Wextra;-Wshadow;-Wno-noexcept-type;-Wno-attributes;-Wno-psabi;-Wno-unknown-warning-option"
	)
	if(${EAGINE_DEBUG})
		set_property(
			TARGET ${TARGET_NAME}
			APPEND PROPERTY COMPILE_OPTIONS
			"-fstack-protector-all"
		)
	endif()
endfunction()

add_custom_target(eagine-std-module)

function(eagine_build_gnu_std_header_module HEADER_NAME)
	add_custom_target(
		eagine-std-${HEADER_NAME}
		COMMAND ${CMAKE_CXX_COMPILER}
			-std=c++${CMAKE_CXX_STANDARD}
			-fmodules-ts
			${CMAKE_CXX_FLAGS}
			-xc++-system-header ${HEADER_NAME}
	)
	add_dependencies(eagine-std-module eagine-std-${HEADER_NAME})
endfunction()

foreach(
	HEADER_NAME
	algorithm
	array
	atomic
	cctype
	cerrno
	charconv
	chrono
	climits
	cmath
	compare
	complex
	concepts
	condition_variable
	csignal
	cstddef
	cstdint
	cstdio
	cstdlib
	cstring
	exception
	filesystem
	fstream
	functional
	initializer_list
	iomanip
	iosfwd
	iostream
	istream
	iterator
	limits
	locale
	map
	memory
	mutex
	new
	numeric
	optional
	ostream
	queue
	random
	ratio
	regex
	set
	sstream
	stack
	stdexcept
	string
	string_view
	system_error
	thread
	tuple
	typeinfo
	type_traits
	utility
	variant
	vector
)
	eagine_build_gnu_std_header_module(${HEADER_NAME})
endforeach()


function(eagine_add_module EAGINE_MODULE_PROPER)
	set(ARG_FLAGS)
	set(ARG_VALUES PARTITION)
	set(ARG_LISTS
		INTERFACES
		SOURCES
		PARTITIONS
		IMPORTS
		PRIVATE_INCLUDE_DIRECTORIES
		PRIVATE_LINK_LIBRARIES
	)
	cmake_parse_arguments(
		EAGINE_MODULE
		"${ARG_FLAGS}" "${ARG_VALUES}" "${ARG_LISTS}"
		${ARGN}
	)

	if("${EAGINE_MODULE_PARTITION}" STREQUAL "")
		set(EAGINE_MODULE_TARGET "${EAGINE_MODULE_PROPER}")
	else()
		set(EAGINE_MODULE_TARGET "${EAGINE_MODULE_PROPER}.${EAGINE_MODULE_PARTITION}")
	endif()

	if(NOT ${EAGINE_MODULE_PARTITION} IN_LIST EAGINE_MODULE_INTERFACES)
		if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${EAGINE_MODULE_PARTITION}.cpp")
			list(APPEND EAGINE_MODULE_INTERFACES ${EAGINE_MODULE_PARTITION})
		elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${EAGINE_MODULE_PARTITION}.cpp")
			list(APPEND EAGINE_MODULE_INTERFACES ${EAGINE_MODULE_PARTITION})
		endif()
	endif()

	unset(EAGINE_MODULE_INTERFACE_FILES)
	foreach(INTERFACE ${EAGINE_MODULE_INTERFACES})
		if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${INTERFACE}.cpp")
			list(
				APPEND EAGINE_MODULE_INTERFACE_FILES
				"${CMAKE_CURRENT_BINARY_DIR}/${INTERFACE}.cpp"
			)
		elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${INTERFACE}.cpp")
			list(
				APPEND EAGINE_MODULE_INTERFACE_FILES
				"${CMAKE_CURRENT_SOURCE_DIR}/${INTERFACE}.cpp"
			)
		endif()
	endforeach()

	unset(EAGINE_MODULE_SOURCE_FILES)
	foreach(SOURCE ${EAGINE_MODULE_SOURCES})
		if("${EAGINE_MODULE_PARTITION}" STREQUAL "")
			if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${SOURCE}.cpp")
				list(
					APPEND EAGINE_MODULE_SOURCE_FILES
					"${CMAKE_CURRENT_BINARY_DIR}/${SOURCE}.cpp"
				)
			elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE}.cpp")
				list(
					APPEND EAGINE_MODULE_SOURCE_FILES
					"${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE}.cpp"
				)
			endif()
		else()
			message(
				FATAL_ERROR
				"SOURCES should not be specified with module PARTITION "
				"(${EAGINE_MODULE_PROPER}:${EAGINE_MODULE_PARTITION})"
			)
		endif()
	endforeach()

	if("${EAGINE_MODULE_PARTITION}" STREQUAL "")
		add_library(
			${EAGINE_MODULE_TARGET} STATIC
			${EAGINE_MODULE_INTERFACE_FILES}
		)
		set_property(
			TARGET ${EAGINE_MODULE_TARGET}
			APPEND PROPERTY EAGINE_MODULE_ID
			"${EAGINE_MODULE_PROPER}"
		)
	else()
		add_library(
			${EAGINE_MODULE_TARGET} OBJECT
			${EAGINE_MODULE_INTERFACE_FILES}
			${EAGINE_MODULE_SOURCE_FILES}
		)
		set_property(
			TARGET ${EAGINE_MODULE_TARGET}
			APPEND PROPERTY EAGINE_MODULE_PARTITION
			"${EAGINE_MODULE_PARTITION}"
		)
		set_property(
			TARGET ${EAGINE_MODULE_TARGET}
			APPEND PROPERTY EAGINE_MODULE_ID
			"${EAGINE_MODULE_PROPER}:${EAGINE_MODULE_PARTITION}"
		)
	endif()
	add_dependencies(${EAGINE_MODULE_TARGET} eagine-std-module)

	set_property(
		TARGET ${EAGINE_MODULE_TARGET}
		APPEND PROPERTY EAGINE_MODULE_PROPER
		"${EAGINE_MODULE_PROPER}"
	)
	eagine_add_module_common_properties(${EAGINE_MODULE_TARGET})

	set_property(
		TARGET ${EAGINE_MODULE_TARGET}
		APPEND PROPERTY EAGINE_MODULE_INTERFACES
		${EAGINE_MODULE_INTERFACES}
	)
	set_property(
		TARGET ${EAGINE_MODULE_TARGET}
		APPEND PROPERTY EAGINE_MODULE_PARTITIONS
		${EAGINE_MODULE_PARTITIONS}
	)

	foreach(PARTITION ${EAGINE_MODULE_PARTITIONS})
		if(NOT TARGET ${EAGINE_MODULE_TARGET}.${PARTITION})
			if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${PARTITION}.cpp")
				eagine_add_module(
					${EAGINE_MODULE_TARGET}
					PARTITION ${PARTITION}
					INTERFACES ${PARTITION}
				)
			endif()
		endif()
		target_link_libraries(
			${EAGINE_MODULE_TARGET}
			PUBLIC ${EAGINE_MODULE_TARGET}.${PARTITION}
		)
	endforeach()

	foreach(IMPORT ${EAGINE_MODULE_IMPORTS})
		if(TARGET ${IMPORT})
			target_link_libraries(
				${EAGINE_MODULE_TARGET}
				PUBLIC ${IMPORT}
			)
		elseif(TARGET ${EAGINE_MODULE_PROPER}.${IMPORT})
			target_link_libraries(
				${EAGINE_MODULE_TARGET}
				PUBLIC ${EAGINE_MODULE_PROPER}.${IMPORT}
			)
		endif()
	endforeach()

endfunction()

function(eagine_target_modules TARGET_NAME)
	eagine_add_module_common_properties(${TARGET_NAME})

	foreach(EAGINE_MODULE_SOURCE ${ARGN})
		target_link_libraries(
			${TARGET_NAME}
			PUBLIC ${EAGINE_MODULE_SOURCE}
		)
	endforeach()
endfunction()

