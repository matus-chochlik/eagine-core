# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt

define_property(
	TARGET
	PROPERTY EAGINE_MODULE_PCM_PATH
	BRIEF_DOCS "Module PCM path"
	FULL_DOCS "Clang C++ modules PCM file path for module target"
)

function(eagine_append_single_module_pcms)
	set(ARG_VALUES SOURCE TARGET)
	cmake_parse_arguments(EAGINE_MODULE "" "${ARG_VALUES}" "" ${ARGN})

	get_property(
		PCM_PATH
		TARGET ${EAGINE_MODULE_SOURCE}
		PROPERTY EAGINE_MODULE_PCM_PATH
	)
	get_property(
		MODULE_ID
		TARGET ${EAGINE_MODULE_SOURCE}
		PROPERTY EAGINE_MODULE_ID
	)
	set_property(
		TARGET ${EAGINE_MODULE_TARGET}
		APPEND PROPERTY COMPILE_OPTIONS
		"-fmodule-file=${MODULE_ID}=${PCM_PATH}"
	)
endfunction()

function(eagine_depend_single_module_pcms)
	set(ARG_VALUES SOURCE TARGET)
	cmake_parse_arguments(EAGINE_MODULE "" "${ARG_VALUES}" "" ${ARGN})

	add_dependencies(
		${EAGINE_MODULE_TARGET}-imports
		${EAGINE_MODULE_SOURCE}-pcm
	)
endfunction()

function(eagine_append_module_pcms)
	set(ARG_VALUES SOURCE TARGET)
	cmake_parse_arguments(EAGINE_MODULE "" "${ARG_VALUES}" "" ${ARGN})

	eagine_append_single_module_pcms(
		TARGET ${EAGINE_MODULE_TARGET}
		SOURCE ${EAGINE_MODULE_SOURCE}
	)

	unset(EAGINE_MODULE_FRAGMENTS)
	get_property(
		EAGINE_MODULE_FRAGMENTS
		TARGET ${EAGINE_MODULE_SOURCE}
		PROPERTY EAGINE_MODULE_FRAGMENTS
	)
	foreach(FRAGMENT ${EAGINE_MODULE_FRAGMENTS})
		eagine_append_module_pcms(
			TARGET ${EAGINE_MODULE_TARGET}
			SOURCE ${EAGINE_MODULE_SOURCE}.${FRAGMENT}
		)
		eagine_depend_single_module_pcms(
			TARGET ${EAGINE_MODULE_TARGET}
			SOURCE ${EAGINE_MODULE_SOURCE}.${FRAGMENT}
		)
	endforeach()

	unset(EAGINE_MODULE_FRAGMENTS)
	get_property(
		EAGINE_MODULE_IMPORTS
		TARGET ${EAGINE_MODULE_SOURCE}
		PROPERTY EAGINE_MODULE_IMPORTS)
	foreach(IMPORT ${EAGINE_MODULE_IMPORTS})
		eagine_append_module_pcms(
			TARGET ${EAGINE_MODULE_TARGET}
			SOURCE ${IMPORT}
		)
		eagine_depend_single_module_pcms(
			TARGET ${EAGINE_MODULE_TARGET}
			SOURCE ${IMPORT}
		)
	endforeach()
endfunction()

function(eagine_add_module EAGINE_MODULE_PROPER)
	set(ARG_FLAGS)
	set(ARG_VALUES FRAGMENT)
	set(ARG_LISTS INTERFACES SOURCES FRAGMENTS IMPORTS)
	cmake_parse_arguments(
		EAGINE_MODULE
		"${ARG_FLAGS}" "${ARG_VALUES}" "${ARG_LISTS}"
		${ARGN}
	)
	if("${EAGINE_MODULE_FRAGMENT}" STREQUAL "")
		set(EAGINE_MODULE_TARGET "${EAGINE_MODULE_PROPER}")
	else()
		set(EAGINE_MODULE_TARGET "${EAGINE_MODULE_PROPER}.${EAGINE_MODULE_FRAGMENT}")
	endif()

	unset(EAGINE_MODULE_INTERFACE_FILES)
	foreach(INTERFACE ${EAGINE_MODULE_INTERFACES})
		list(
			APPEND EAGINE_MODULE_INTERFACE_FILES
			"${CMAKE_CURRENT_SOURCE_DIR}/${INTERFACE}.cpp"
		)
	endforeach()

	unset(EAGINE_MODULE_SOURCE_FILES)
	foreach(SOURCE ${EAGINE_MODULE_SOURCES})
		list(
			APPEND EAGINE_MODULE_SOURCE_FILES
			"${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE}.cpp"
		)
	endforeach()

	if("${EAGINE_MODULE_FRAGMENT}" STREQUAL "")
		add_library(
			${EAGINE_MODULE_TARGET} STATIC
			${EAGINE_MODULE_INTERFACE_FILES}
			${EAGINE_MODULE_SOURCE_FILES}
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
			APPEND PROPERTY EAGINE_MODULE_FRAGMENT
			"${EAGINE_MODULE_FRAGMENT}"
		)
		set_property(
			TARGET ${EAGINE_MODULE_TARGET}
			APPEND PROPERTY EAGINE_MODULE_ID
			"${EAGINE_MODULE_PROPER}:${EAGINE_MODULE_FRAGMENT}"
		)
	endif()
	set_property(
		TARGET ${EAGINE_MODULE_TARGET}
		APPEND PROPERTY EAGINE_MODULE_PROPER
		"${EAGINE_MODULE_PROPER}"
	)

	set_property(
		TARGET ${EAGINE_MODULE_TARGET}
		APPEND PROPERTY COMPILE_OPTIONS
		"-fmodules"
		"-fbuiltin-module-map"
	)
	set_property(
		TARGET ${EAGINE_MODULE_TARGET}
		APPEND PROPERTY EAGINE_MODULE_INTERFACES
		${EAGINE_MODULE_INTERFACES}
	)
	set_property(
		TARGET ${EAGINE_MODULE_TARGET}
		APPEND PROPERTY EAGINE_MODULE_FRAGMENTS
		${EAGINE_MODULE_FRAGMENTS}
	)
	set_property(
		TARGET ${EAGINE_MODULE_TARGET}
		APPEND PROPERTY EAGINE_MODULE_IMPORTS
		${EAGINE_MODULE_IMPORTS}
	)
	set_property(
		TARGET ${EAGINE_MODULE_TARGET}
		APPEND PROPERTY EAGINE_MODULE_PCM_PATH
		"${CMAKE_CURRENT_BINARY_DIR}/${EAGINE_MODULE_TARGET}.pcm"
	)

	foreach(FRAGMENT ${EAGINE_MODULE_FRAGMENTS})
		if(NOT TARGET ${EAGINE_MODULE_TARGET}.${FRAGMENT})
			if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${FRAGMENT}.cpp")
				eagine_add_module(
					${EAGINE_MODULE_TARGET}
					FRAGMENT ${FRAGMENT}
					INTERFACES ${FRAGMENT}
				)
			endif()
		endif()
		target_link_libraries(
			${EAGINE_MODULE_TARGET}
			PUBLIC ${EAGINE_MODULE_TARGET}.${FRAGMENT}
		)
	endforeach()

	foreach(IMPORT ${EAGINE_MODULE_IMPORTS})
		target_link_libraries(
			${EAGINE_MODULE_TARGET}
			PUBLIC ${IMPORT}
		)
	endforeach()

	add_custom_target(${EAGINE_MODULE_TARGET}-imports)

	eagine_append_module_pcms(
		SOURCE ${EAGINE_MODULE_TARGET}
		TARGET ${EAGINE_MODULE_TARGET}
	)

	get_property(
		PCM_COMPILE_OPTIONS
		TARGET ${EAGINE_MODULE_TARGET}
		PROPERTY COMPILE_OPTIONS
	)

	add_custom_command(
		OUTPUT ${EAGINE_MODULE_TARGET}.pcm
		COMMAND ${CMAKE_CXX_COMPILER}
		ARGS
			-std=c++${CMAKE_CXX_STANDARD}
			${CMAKE_CXX_FLAGS}
			${PCM_COMPILE_OPTIONS}
			-Xclang -emit-module-interface
			-c ${EAGINE_MODULE_INTERFACE_FILES}
			-o ${EAGINE_MODULE_TARGET}.pcm
		DEPENDS
			${EAGINE_MODULE_INTERFACE_FILES}
			${EAGINE_MODULE_TARGET}-imports
		)
	add_custom_target(
		${EAGINE_MODULE_TARGET}-pcm
		DEPENDS ${EAGINE_MODULE_TARGET}.pcm
	)
	add_dependencies(${EAGINE_MODULE_TARGET} ${EAGINE_MODULE_TARGET}-pcm)
endfunction()

function(eagine_target_module TARGET_NAME EAGINE_MODULE_SOURCE)
	target_link_libraries(${TARGET_NAME} PUBLIC ${EAGINE_MODULE_SOURCE})

	set_property(
		TARGET ${TARGET_NAME}
		APPEND PROPERTY COMPILE_OPTIONS
		"-fmodules"
		"-fbuiltin-module-map"
	)
	add_custom_target(${TARGET_NAME}-imports)

	eagine_append_module_pcms(
		TARGET ${TARGET_NAME}
		SOURCE ${EAGINE_MODULE_SOURCE}
	)
endfunction()

