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
# ------------------------------------------------------------------------------
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

	unset(COMPILE_OPTS)
	get_property(
		COMPILE_OPTS
		TARGET ${EAGINE_MODULE_TARGET}
		PROPERTY COMPILE_OPTIONS
	)
	set(OPTION "-fmodule-file=${MODULE_ID}=${PCM_PATH}")
	if(NOT "${OPTION}" IN_LIST COMPILE_OPTS)
		set_property(
			TARGET ${EAGINE_MODULE_TARGET}
			APPEND PROPERTY COMPILE_OPTIONS
				"${OPTION}"
		)
	endif()
endfunction()
# ------------------------------------------------------------------------------
function(eagine_depend_single_module_pcms)
	set(ARG_VALUES SOURCE TARGET)
	cmake_parse_arguments(EAGINE_MODULE "" "${ARG_VALUES}" "" ${ARGN})

	add_dependencies(
		${EAGINE_MODULE_TARGET}-imports
		${EAGINE_MODULE_SOURCE}-pcm
	)
endfunction()
# ------------------------------------------------------------------------------
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
# ------------------------------------------------------------------------------
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
# ------------------------------------------------------------------------------
function(eagine_add_module_common_properties TARGET_NAME)
	set_property(
		TARGET ${TARGET_NAME}
		APPEND PROPERTY COMPILE_OPTIONS
		"-fmodules"
		"-fexperimental-library"
	)
	set_property(
		TARGET ${TARGET_NAME}
		APPEND PROPERTY PRIVATE_COMPILE_OPTIONS
		"-Weverything;-Wno-sign-conversion;-Wno-old-style-cast;-Wno-c++98-compat;-Wno-c++98-compat-pedantic;-Wno-c++20-compat;-Wno-undef;-Wno-double-promotion;-Wno-global-constructors;-Wno-exit-time-destructors;-Wno-date-time;-Wno-padded;-Wno-missing-prototypes;-Wno-undefined-inline;-Wno-documentation-unknown-command;-Wno-switch-enum;-Wno-ctad-maybe-unsupported;-Wno-used-but-marked-unused;-Wno-c++1z-extensions;-Wno-c++2b-extensions"
	)
	if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
		set_property(
			TARGET ${TARGET_NAME}
			APPEND PROPERTY COMPILE_OPTIONS
			"-fstack-protector-all"
		)
	endif()
endfunction()
# ------------------------------------------------------------------------------
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
			if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${SOURCE}_impl.cpp")
				list(
					APPEND EAGINE_MODULE_SOURCE_FILES
					"${CMAKE_CURRENT_BINARY_DIR}/${SOURCE}_impl.cpp"
				)
			elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE}_impl.cpp")
				list(
					APPEND EAGINE_MODULE_SOURCE_FILES
					"${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE}_impl.cpp"
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
		if(NOT "${EAGINE_MODULE_SOURCE_FILES}" STREQUAL "")
			add_library(
				${EAGINE_MODULE_TARGET}-implement OBJECT
				${EAGINE_MODULE_SOURCE_FILES}
			)
			eagine_add_module_common_properties(${EAGINE_MODULE_TARGET}-implement)

			foreach(DIR ${EAGINE_MODULE_PRIVATE_INCLUDE_DIRECTORIES})
				target_include_directories(
					${EAGINE_MODULE_TARGET}-implement
					PRIVATE ${DIR}
				)
			endforeach()
			target_link_libraries(
				${EAGINE_MODULE_TARGET}
				PUBLIC ${EAGINE_MODULE_TARGET}-implement
			)
		foreach(INTF ${EAGINE_MODULE_PARTITIONS})
				set(PCM_PATH
					"${CMAKE_CURRENT_BINARY_DIR}/${EAGINE_MODULE_PROPER}.${INTF}.pcm"
				)
				set_property(
					TARGET ${EAGINE_MODULE_TARGET}-implement
					APPEND PROPERTY COMPILE_OPTIONS
					"-fmodule-file=${EAGINE_MODULE_PROPER}:${INTF}=${PCM_PATH}"
				)
			endforeach()
			foreach(LIBRARY ${EAGINE_MODULE_PRIVATE_LINK_LIBRARIES})
				target_link_libraries(
					${EAGINE_MODULE_TARGET}-implement
					PRIVATE ${LIBRARY}
				)
			endforeach()
		else()
			foreach(DIR ${EAGINE_MODULE_PRIVATE_INCLUDE_DIRECTORIES})
				target_include_directories(
					${EAGINE_MODULE_TARGET}
					PRIVATE ${DIR}
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
	foreach(IMPORT ${EAGINE_MODULE_IMPORTS})
		if(TARGET ${IMPORT})
			set(EAGINE_MODULE_IMPORT "${IMPORT}")
		elseif(TARGET ${EAGINE_MODULE_PROPER}.${IMPORT})
			set(EAGINE_MODULE_IMPORT "${EAGINE_MODULE_PROPER}.${IMPORT}")
		endif()
		if(NOT "${EAGINE_MODULE_IMPORT}" STREQUAL "")
			set_property(
				TARGET ${EAGINE_MODULE_TARGET}
				APPEND PROPERTY EAGINE_MODULE_IMPORTS
				${EAGINE_MODULE_IMPORT}
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
	foreach(DEF ${TGT_COMPILE_DEFINITIONS})
		list(APPEND PCM_COMPILE_DEFINITIONS "-D${DEF}")
	endforeach()
	foreach(DEF ${DIR_COMPILE_DEFINITIONS})
		list(APPEND PCM_COMPILE_DEFINITIONS "-D${DEF}")
	endforeach()
	if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
		list(APPEND PCM_COMPILE_OPTIONS "-O3")
		list(APPEND PCM_COMPILE_DEFINITIONS "-DNDEBUG")
	elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
		list(APPEND PCM_COMPILE_OPTIONS "-fstack-protector-all")
	endif()

	unset(PCM_INCLUDE_DIRECTORIES)
	foreach(DIR ${EAGINE_MODULE_PRIVATE_INCLUDE_DIRECTORIES})
		list(APPEND PCM_COMPILE_DEFINITIONS "-I${DIR}")
	endforeach()

	add_custom_command(
		OUTPUT ${EAGINE_MODULE_TARGET}.pcm
		COMMAND ${CMAKE_CXX_COMPILER}
		ARGS
			-std=c++${CMAKE_CXX_STANDARD}
			${CMAKE_CXX_FLAGS}
			${PCM_COMPILE_OPTIONS}
			${PCM_COMPILE_DEFINITIONS}
			${PCM_INCLUDE_DIRECTORIES}
			-Xclang -emit-module-interface
			-c ${EAGINE_MODULE_INTERFACE_FILES}
			-o ${EAGINE_MODULE_TARGET}.pcm
		DEPENDS
			${EAGINE_MODULE_TARGET}-imports
			${EAGINE_MODULE_INTERFACE_FILES}
		)
	add_custom_target(
		${EAGINE_MODULE_TARGET}-pcm
		SOURCES ${EAGINE_MODULE_TARGET}.pcm
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
# ------------------------------------------------------------------------------
function(eagine_target_modules TARGET_NAME)
	eagine_add_module_common_properties(${TARGET_NAME})

	foreach(EAGINE_MODULE_SOURCE ${ARGN})
		unset(EAGINE_TARGET_IMPORTS)
		list(APPEND EAGINE_TARGET_IMPORTS ${EAGINE_MODULE_SOURCE})
		unset(EAGINE_MODULE_PARTITIONS)
		get_property(
			EAGINE_MODULE_PARTITIONS
			TARGET ${EAGINE_MODULE_SOURCE}
			PROPERTY EAGINE_MODULE_PARTITIONS
		)
		foreach(PARTITION ${EAGINE_MODULE_PARTITIONS})
			unset(EAGINE_MODULE_IMPORTS)
			get_property(
				EAGINE_MODULE_IMPORTS
				TARGET ${EAGINE_MODULE_SOURCE}
				PROPERTY EAGINE_MODULE_IMPORTS)
			foreach(IMPORT ${EAGINE_MODULE_IMPORTS})
				list(APPEND EAGINE_TARGET_IMPORTS ${IMPORT})
			endforeach()
		endforeach()
		
		list(REMOVE_DUPLICATES EAGINE_TARGET_IMPORTS)
		foreach(IMPORT ${EAGINE_TARGET_IMPORTS})
			target_link_libraries(
				${TARGET_NAME}
				PUBLIC ${IMPORT}
			)
		endforeach()


		if(NOT TARGET ${TARGET_NAME}-imports)
			add_custom_target(${TARGET_NAME}-imports)
		endif()

		eagine_append_module_pcms(
			TARGET ${TARGET_NAME}
			SOURCE ${EAGINE_MODULE_SOURCE}
		)
	endforeach()
endfunction()
# ------------------------------------------------------------------------------
function(eagine_add_module_tests EAGINE_MODULE_PROPER)
	set(ARG_FLAGS)
	set(ARG_VALUES PARTITION)
	set(ARG_LISTS
		UNITS
		IMPORTS
	)
	cmake_parse_arguments(
		EAGINE_MODULE_TEST
		"${ARG_FLAGS}" "${ARG_VALUES}" "${ARG_LISTS}"
		${ARGN}
	)
	add_test(
		NAME "build-${EAGINE_MODULE_PROPER}"
		COMMAND "${CMAKE_COMMAND}"
			--build "${CMAKE_BINARY_DIR}"
			--target "${EAGINE_MODULE_PROPER}"
	)
	set_tests_properties(
		"build-${EAGINE_MODULE_PROPER}"
		PROPERTIES
			TIMEOUT 200
			RUN_SERIAL True
			FIXTURES_SETUP
				"${EAGINE_MODULE_PROPER}-built"
	)
	unset(EAGINE_MODULE_PROFRAW_FILES)
	unset(EAGINE_MODULE_PROFRAW_FIXTURES)
	foreach(UNIT ${EAGINE_MODULE_TEST_UNITS})
		if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${UNIT}_test.cpp")
			set(TEST_NAME test.${EAGINE_MODULE_PROPER}.${UNIT})
			add_executable(${TEST_NAME} EXCLUDE_FROM_ALL "${UNIT}_test.cpp")
			target_compile_options(
				${TEST_NAME}
				PRIVATE
					-fprofile-instr-generate
					-fcoverage-mapping
					-mllvm
					-runtime-counter-relocation
					-Wno-unsafe-buffer-usage
			)
			target_link_options(
				${TEST_NAME}
				PRIVATE
					-fprofile-instr-generate
					-fcoverage-mapping
			)
			target_link_libraries(
				${TEST_NAME}
				PRIVATE
					eagine-core-testing-headers
			)
			eagine_add_exe_analysis(${TEST_NAME})
			eagine_target_modules(${TEST_NAME} ${EAGINE_MODULE_PROPER})

			foreach(IMPORT ${EAGINE_MODULE_TEST_IMPORTS})
				eagine_target_modules(${TEST_NAME} ${IMPORT})
			endforeach()

			set_target_properties(${TEST_NAME} PROPERTIES FOLDER "Test/Core")

			add_test(
				NAME "build-${TEST_NAME}"
				COMMAND "${CMAKE_COMMAND}"
					--build "${CMAKE_BINARY_DIR}"
					--target "${TEST_NAME}"
			)
			set_tests_properties(
				"build-${TEST_NAME}"
				PROPERTIES
					TIMEOUT 180
					FIXTURES_SETUP
						"${TEST_NAME}-built"
					FIXTURES_REQUIRED
						"${EAGINE_MODULE_PROPER}-built"
			)
			add_test(
				NAME "execute-${TEST_NAME}"
				COMMAND "${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}"
			)
			set_tests_properties(
				"execute-${TEST_NAME}"
				PROPERTIES
					TIMEOUT 300
					ENVIRONMENT
						"LLVM_PROFILE_FILE=${EAGINE_MODULE_PROPER}.${UNIT}.profraw"
					FIXTURES_SETUP
						"${TEST_NAME}-profraw"
					FIXTURES_REQUIRED
						"${TEST_NAME}-built"
			)
			list(APPEND
				EAGINE_MODULE_PROFRAW_FILES
				"${CMAKE_CURRENT_BINARY_DIR}/${EAGINE_MODULE_PROPER}.${UNIT}.profraw"
			)
			list(APPEND EAGINE_MODULE_PROFRAW_FIXTURES "${TEST_NAME}-profraw")
		else()
			message(
				FATAL_ERROR
				"Missing source for unit test ${EAGINE_MODULE_PROPER}.${UNIT}!"
			)
		endif()
	endforeach()
	if(LLVM_PROFDATA)
		add_test(
			NAME "profdata-${EAGINE_MODULE_PROPER}"
			COMMAND "${LLVM_PROFDATA}" merge
				-o "${EAGINE_MODULE_PROPER}.profdata"
				-sparse ${EAGINE_MODULE_PROFRAW_FILES}
		)
		set_tests_properties(
			"profdata-${EAGINE_MODULE_PROPER}"
			PROPERTIES
				TIMEOUT 60
				FIXTURES_REQUIRED "${EAGINE_MODULE_PROFRAW_FIXTURES}"
		)
	endif()
endfunction()
