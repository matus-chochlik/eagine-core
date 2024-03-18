#  Copyright Matus Chochlik.
#  Distributed under the Boost Software License, Version 1.0.
#  See accompanying file LICENSE_1_0.txt or copy at
#  https://www.boost.org/LICENSE_1_0.txt
#
function(eagine_add_target_resource_file)
	cmake_parse_arguments(
		EAGINE_RESOURCE
		"PACKED;ENABLE_SEARCH"
		"TARGET"
		"INPUT_FILES;INPUT_TARGETS;SEARCH_PATHS;SEARCH_IDENTIFIERS;RESOURCES"
		${ARGN}
	)

	set(RESOURCE_FILE ${EAGINE_RESOURCE_TARGET}.resources.cpp)
	set(GEN_DEPENDS)
	set(GEN_OPTIONS)
	if(${EAGINE_USE_ZLIB})
		if(${EAGINE_RESOURCE_PACKED})
			list(APPEND GEN_OPTIONS --packed)
		endif()
	endif()
	list(APPEND GEN_OPTIONS -o)
	list(APPEND GEN_OPTIONS ${CMAKE_CURRENT_BINARY_DIR}/${RESOURCE_FILE})
	unset(RES_ID)
	foreach(ID_OR_RES_FILE ${EAGINE_RESOURCE_RESOURCES})
		if("${RES_ID}" STREQUAL "")
			if("${ID_OR_RES_FILE}" MATCHES "^[A-Za-z0-9_]+$")
				set(RES_ID "${ID_OR_RES_FILE}")
			else()
				message(FATAL_ERROR "Invalid resource identifier '${ID_OR_RES_FILE}'")
			endif()
		else()
			list(APPEND GEN_OPTIONS -I)
			list(APPEND GEN_OPTIONS ${RES_ID})
			if(TARGET ${RES_NAME})
				get_property(
					RES_PATH
					TARGET ${ID_OR_RES_FILE}
					PROPERTY EAGINE_BAKED_RESOURCE_PATH
				)
				list(APPEND GEN_OPTIONS ${ID_OR_RES_FILE})
				list(APPEND GEN_DEPENDS "${RES_PATH}")
				list(APPEND GEN_OPTIONS -t)
				list(APPEND GEN_OPTIONS ${ID_OR_RES_FILE} ${RES_PATH})
			else()
				if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${ID_OR_RES_FILE}")
					list(APPEND GEN_OPTIONS "${CMAKE_CURRENT_SOURCE_DIR}/${ID_OR_RES_FILE}")
					list(APPEND GEN_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${ID_OR_RES_FILE}")
				elseif(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${ID_OR_RES_FILE}")
					list(APPEND GEN_OPTIONS "${CMAKE_CURRENT_BINARY_DIR}/${ID_OR_RES_FILE}")
					list(APPEND GEN_DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${ID_OR_RES_FILE}")
				else()
					list(APPEND GEN_OPTIONS ${ID_OR_RES_FILE})
					list(APPEND GEN_DEPENDS "${ID_OR_RES_FILE}")
				endif()
			endif()
			unset(RES_ID)
		endif()
	endforeach()
	foreach(SEARCH_PATH ${EAGINE_RESOURCE_SEARCH_PATHS})
		list(APPEND GEN_OPTIONS -s)
		list(APPEND GEN_OPTIONS ${SEARCH_PATH})
		foreach(SRC_FILE ${EAGINE_RESOURCE_INPUT_FILES})
			if(EXISTS ${SRC_FILE})
				list(APPEND GEN_OPTIONS -i)
				list(APPEND GEN_OPTIONS ${SRC_FILE})
				list(APPEND GEN_DEPENDS ${SRC_FILE})
			elseif(EXISTS ${SEARCH_PATH}/${SRC_FILE})
				list(APPEND GEN_OPTIONS -i)
				list(APPEND GEN_OPTIONS ${SEARCH_PATH}/${SRC_FILE})
				list(APPEND GEN_DEPENDS ${SEARCH_PATH}/${SRC_FILE})
			endif()
		endforeach()
	endforeach()
	foreach(SEARCH_IDENT ${EAGINE_RESOURCE_SEARCH_IDENTIFIERS})
		list(APPEND GEN_OPTIONS -g)
		list(APPEND GEN_OPTIONS ${SEARCH_IDENT})
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
	if(${EAGINE_RESOURCE_ENABLE_SEARCH})
		set_target_properties(
			${EAGINE_RESOURCE_TARGET}
			PROPERTIES LINK_FLAGS -rdynamic
		)
		list(APPEND GEN_OPTIONS --enable-search)
	endif()

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
	if(TARGET ${EAGINE_RESOURCE_TARGET}-objects)
		target_sources(
			${EAGINE_RESOURCE_TARGET}-objects
			PRIVATE ${RESOURCE_FILE}
		)
	endif()
endfunction()

function(eagine_embed_target_resources)
	cmake_parse_arguments(EAGINE_RESOURCE "" "TARGET" "SCAN_TARGETS" ${ARGN})
	if(NOT EAGINE_RESOURCE_TARGET)
		message(FATAL_ERROR "Missing required argument TARGET")
	endif()

	unset(EAGINE_RESOURCE_ALL_TARGET_SOURCES)
	foreach(PROP SOURCES EAGINE_MODULE_INTERFACES)
		get_target_property(
			TARGET_SOURCES
			${EAGINE_RESOURCE_TARGET}
			${PROP}
		)
		if(TARGET_SOURCES)
			list(APPEND EAGINE_RESOURCE_ALL_TARGET_SOURCES ${TARGET_SOURCES})
		endif()
	endforeach()

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

	add_dependencies(
		${EAGINE_RESOURCE_TARGET}
		eagine.core eagine.core.resource
	)
	if(TARGET ${EAGINE_RESOURCE_TARGET}-objects)
		add_dependencies(
			${EAGINE_RESOURCE_TARGET}-objects
			eagine.core eagine.core.resource
		)
	endif()

	if(${EAGINE_CLANGXX_COMPILER})
		if(${EAGINE_EMBED_INSTALLED})
			# TODO
		else()
			set_property(
				TARGET ${EAGINE_RESOURCE_TARGET}
				APPEND PROPERTY COMPILE_OPTIONS
				"-fprebuilt-module-path=${EAGINE_CORE_BINARY_ROOT}/source/modules/eagine;-fprebuilt-module-path=${EAGINE_CORE_BINARY_ROOT}/source/modules/eagine/resource")
		endif()
	endif()
endfunction()

