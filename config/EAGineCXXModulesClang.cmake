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
	if("${PCM_PATH}" MATCHES "${MODULE_ID}.pcm$")
		set_property(
			TARGET ${EAGINE_MODULE_TARGET}
			APPEND PROPERTY COMPILE_OPTIONS
			"-fmodule-file=${PCM_PATH}"
		)
	else()
		set_property(
			TARGET ${EAGINE_MODULE_TARGET}
			APPEND PROPERTY COMPILE_OPTIONS
			"-fmodule-file=${MODULE_ID}=${PCM_PATH}"
		)
	endif()
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

	unset(EAGINE_MODULE_IMPORTS)
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

function(eagine_append_own_module_pcms)
	set(ARG_VALUES SOURCE TARGET)
	cmake_parse_arguments(EAGINE_MODULE "" "${ARG_VALUES}" "" ${ARGN})

	unset(EAGINE_MODULE_IMPORTS)
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

	unset(EAGINE_MODULE_PARTITIONS)
	get_property(
		EAGINE_MODULE_PARTITIONS
		TARGET ${EAGINE_MODULE_SOURCE}
		PROPERTY EAGINE_MODULE_PARTITIONS
	)
	foreach(PARTITION ${EAGINE_MODULE_PARTITIONS})
		eagine_append_module_pcms(
			TARGET ${EAGINE_MODULE_TARGET}
			SOURCE ${EAGINE_MODULE_SOURCE}.${PARTITION}
		)
		eagine_depend_single_module_pcms(
			TARGET ${EAGINE_MODULE_TARGET}
			SOURCE ${EAGINE_MODULE_SOURCE}.${PARTITION}
		)
	endforeach()
endfunction()

function(eagine_add_module EAGINE_MODULE_PROPER)
	set(ARG_FLAGS)
	set(ARG_VALUES PARTITION)
	set(ARG_LISTS INTERFACES SOURCES PARTITIONS IMPORTS PRIVATE_LINK_LIBRARIES)
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
		if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${EAGINE_MODULE_PARTITION}.cpp")
			list(APPEND EAGINE_MODULE_INTERFACES ${EAGINE_MODULE_PARTITION})
		endif()
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
		if("${EAGINE_MODULE_PARTITION}" STREQUAL "")
			list(
				APPEND EAGINE_MODULE_SOURCE_FILES
				"${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE}.cpp"
			)
		else()
			message(
				FATAL_ERROR
				"SOURCES hould not be specified with module PARTITION "
				"(${EAGINE_MODULE_PROPER}:${EAGINE_MODULE_PARTITION})"
			)
		endif()
	endforeach()

	if("${EAGINE_MODULE_PARTITION}" STREQUAL "")
		add_library(
			${EAGINE_MODULE_TARGET} STATIC
			${EAGINE_MODULE_INTERFACE_FILES}
		)
		if(NOT "${EAGINE_MODULE_SOURCE_FILES}" STREQUAL "")
			add_library(
				${EAGINE_MODULE_TARGET}-implement OBJECT
				${EAGINE_MODULE_SOURCE_FILES}
			)
			set_property(
				TARGET ${EAGINE_MODULE_TARGET}-implement
				APPEND PROPERTY COMPILE_OPTIONS
				"-fmodules"
				"-fbuiltin-module-map"
			)
			target_link_libraries(
				${EAGINE_MODULE_TARGET}
				PUBLIC ${EAGINE_MODULE_TARGET}-implement
			)
			foreach(LIBRARY ${EAGINE_MODULE_PRIVATE_LINK_LIBRARIES})
				target_link_libraries(
					${EAGINE_MODULE_TARGET}-implement
					PRIVATE ${LIBRARY}
				)
			endforeach()
		endif()
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
		APPEND PROPERTY EAGINE_MODULE_PARTITIONS
		${EAGINE_MODULE_PARTITIONS}
	)
	foreach(IMPORT ${EAGINE_MODULE_IMPORTS})
		if(TARGET ${IMPORT})
			set_property(
				TARGET ${EAGINE_MODULE_TARGET}
				APPEND PROPERTY EAGINE_MODULE_IMPORTS
				${IMPORT}
			)
		elseif(TARGET ${EAGINE_MODULE_PROPER}.${IMPORT})
			set_property(
				TARGET ${EAGINE_MODULE_TARGET}
				APPEND PROPERTY EAGINE_MODULE_IMPORTS
				${EAGINE_MODULE_PROPER}.${IMPORT}
			)
		endif()
	endforeach()
	set_property(
		TARGET ${EAGINE_MODULE_TARGET}
		APPEND PROPERTY EAGINE_MODULE_PCM_PATH
		"${CMAKE_CURRENT_BINARY_DIR}/${EAGINE_MODULE_TARGET}.pcm"
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

	add_custom_target(${EAGINE_MODULE_TARGET}-imports)
	eagine_append_own_module_pcms(
		SOURCE ${EAGINE_MODULE_TARGET}
		TARGET ${EAGINE_MODULE_TARGET}
	)
	if(TARGET ${EAGINE_MODULE_TARGET}-implement)
		add_custom_target(
			${EAGINE_MODULE_TARGET}-implement-imports
			DEPENDS ${EAGINE_MODULE_TARGET}-imports
		)
		eagine_append_module_pcms(
			SOURCE ${EAGINE_MODULE_TARGET}
			TARGET ${EAGINE_MODULE_TARGET}-implement
		)
	endif()

	get_property(
		PCM_COMPILE_OPTIONS
		TARGET ${EAGINE_MODULE_TARGET}
		PROPERTY COMPILE_OPTIONS
	)
	get_property(
		TGT_COMPILE_DEFINITIONS
		TARGET ${EAGINE_MODULE_TARGET}
		PROPERTY COMPILE_DEFINITIONS
	)
	get_property(
		DIR_COMPILE_DEFINITIONS
		DIRECTORY
		PROPERTY COMPILE_DEFINITIONS
	)
	unset(PCM_COMPILE_DEFINITIONS)
	foreach(DEF ${DIR_COMPILE_DEFINITIONS})
		list(APPEND PCM_COMPILE_DEFINITIONS "-D${DEF}")
	endforeach()
	foreach(DEF ${TGT_COMPILE_DEFINITIONS})
		list(APPEND PCM_COMPILE_DEFINITIONS "-D${DEF}")
	endforeach()

	add_custom_command(
		OUTPUT ${EAGINE_MODULE_TARGET}.pcm
		COMMAND ${CMAKE_CXX_COMPILER}
		ARGS
			-std=c++${CMAKE_CXX_STANDARD}
			${CMAKE_CXX_FLAGS}
			${PCM_COMPILE_OPTIONS}
			${PCM_COMPILE_DEFINITIONS}
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
	add_dependencies(
		${EAGINE_MODULE_TARGET}
		${EAGINE_MODULE_TARGET}-pcm
	)
	if(TARGET ${EAGINE_MODULE_TARGET}-implement)
		add_dependencies(
			${EAGINE_MODULE_TARGET}-implement
			${EAGINE_MODULE_TARGET}-pcm
		)
	endif()
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
