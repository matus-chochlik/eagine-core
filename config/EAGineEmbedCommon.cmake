#  Copyright Matus Chochlik.
#  Distributed under the Boost Software License, Version 1.0.
#  See accompanying file LICENSE_1_0.txt or copy at
#   http://www.boost.org/LICENSE_1_0.txt
#
function(eagine_add_target_resource_file)
	cmake_parse_arguments(
		EAGINE_RESOURCE
		"PACKED"
		"TARGET"
		"INPUT_FILES;INPUT_TARGETS;SEARCH_PATHS"
		${ARGN}
	)

	set(RESOURCE_FILE ${EAGINE_RESOURCE_TARGET}.resources.cpp)
	set(GEN_DEPENDS)
	set(GEN_OPTIONS)
	if(${ZLIB_FOUND} )
		if(${EAGINE_RESOURCE_PACKED})
			list(APPEND GEN_OPTIONS --packed)
		endif()
	endif()
	list(APPEND GEN_OPTIONS -o)
	list(APPEND GEN_OPTIONS ${CMAKE_CURRENT_BINARY_DIR}/${RESOURCE_FILE})
	foreach(SEARCH_PATH ${EAGINE_RESOURCE_SEARCH_PATHS})
		list(APPEND GEN_OPTIONS -s)
		list(APPEND GEN_OPTIONS ${SEARCH_PATH})
		foreach(RES_FILE ${EAGINE_RESOURCE_INPUT_FILES})
			if(EXISTS ${RES_FILE})
				list(APPEND GEN_OPTIONS -i)
				list(APPEND GEN_OPTIONS ${RES_FILE})
				list(APPEND GEN_DEPENDS ${RES_FILE})
			elseif(EXISTS ${SEARCH_PATH}/${RES_FILE})
				list(APPEND GEN_OPTIONS -i)
				list(APPEND GEN_OPTIONS ${SEARCH_PATH}/${RES_FILE})
				list(APPEND GEN_DEPENDS ${SEARCH_PATH}/${RES_FILE})
			endif()
		endforeach()
	endforeach()

	foreach(RES_NAME ${EAGINE_RESOURCE_INPUT_TARGETS})
		if(TARGET ${RES_NAME})
			get_property(
				RES_PATH
				TARGET ${RES_NAME}
				PROPERTY EAGINE_BAKED_RESOURCE_PATH
			)

			list(APPEND GEN_DEPENDS "${RES_NAME}")
			list(APPEND GEN_OPTIONS -t)
			list(APPEND GEN_OPTIONS ${RES_NAME} ${RES_PATH})
		else()
			list(APPEND GEN_DEPENDS "${RES_NAME}")
		endif()
	endforeach()

	add_custom_command(
		OUTPUT ${RESOURCE_FILE}
		COMMAND ${EAGINE_EMBED_GENERATOR} ${GEN_OPTIONS}
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
		DEPENDS ${GEN_DEPENDS}
	)
	set_source_files_properties(
		${RESOURCE_FILE}
		PROPERTIES GENERATED 1
	)
	target_sources(
		${EAGINE_RESOURCE_TARGET}
		PRIVATE ${RESOURCE_FILE}
	)
endfunction()

function(eagine_embed_target_resources)
	cmake_parse_arguments(EAGINE_RESOURCE "" "TARGET" "SCAN_TARGETS" ${ARGN})
	if(NOT EAGINE_RESOURCE_TARGET)
		message(FATAL_ERROR "Missing required argument TARGET")
	endif()

	unset(EAGINE_RESOURCE_ALL_TARGET_SOURCES)
	get_target_property(
		TARGET_SOURCES
		${EAGINE_RESOURCE_TARGET}
		SOURCES
	)
	if(TARGET_SOURCES)
		list(APPEND EAGINE_RESOURCE_ALL_TARGET_SOURCES ${TARGET_SOURCES})
	endif()

	foreach(SCAN_TARGET ${EAGINE_RESOURCE_TARGET} ${EAGINE_RESOURCE_SCAN_TARGETS})
		get_target_property(
			TARGET_PRIVATE_HEADERS
			${SCAN_TARGET}
			PRIVATE_HEADER
		)
		if(TARGET_PRIVATE_HEADERS)
			list(APPEND EAGINE_RESOURCE_ALL_TARGET_SOURCES ${TARGET_PRIVATE_HEADERS})
		endif()
	endforeach()

	eagine_add_target_resource_file(
		${ARGN}
		INPUT_FILES ${EAGINE_RESOURCE_ALL_TARGET_SOURCES}
		SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}"
	)
endfunction()

