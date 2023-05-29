# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt

define_property(
	TARGET
	PROPERTY EAGINE_MODULE_PCM_PATH
	BRIEF_DOCS "Module PCM path"
	FULL_DOCS "Clang precompiled module path")
define_property(
	TARGET
	PROPERTY EAGINE_MODULE_INTERFACES
	BRIEF_DOCS "Module interface file"
	FULL_DOCS "Path to module interface file")
define_property(
	TARGET
	PROPERTY EAGINE_MODULE_PARTITIONS
	BRIEF_DOCS "Module partition list"
	FULL_DOCS "List of partition names for a module")
# ------------------------------------------------------------------------------
function(eagine_add_module EAGINE_MODULE_PROPER)
	set(ARG_FLAGS)
	set(ARG_VALUES PARTITION)
	set(ARG_LISTS
		SOURCES
		IMPORTS
		SUBMODULES
		PRIVATE_INCLUDE_DIRECTORIES
		PRIVATE_LINK_LIBRARIES)
	cmake_parse_arguments(
		EAGINE_MODULE
		"${ARG_FLAGS}" "${ARG_VALUES}" "${ARG_LISTS}"
		${ARGN})

	set(EAGINE_MODULE_OBJECTS ${EAGINE_MODULE_PROPER}-objects)

	if(NOT TARGET ${EAGINE_MODULE_OBJECTS})
		add_library(${EAGINE_MODULE_OBJECTS} OBJECT)
		set_property(
			TARGET ${EAGINE_MODULE_OBJECTS}
			PROPERTY EAGINE_MODULE_PCM_PATH "${CMAKE_CURRENT_BINARY_DIR}")
		set_property(
			TARGET ${EAGINE_MODULE_OBJECTS}
			PROPERTY LINKER_LANGUAGE CXX)
		target_compile_options(
			${EAGINE_MODULE_OBJECTS}
			PUBLIC
				-fmodules
				-Wno-read-modules-implicitly
				"-fprebuilt-module-path=${CMAKE_CURRENT_BINARY_DIR}")
	endif()

	if(NOT TARGET ${EAGINE_MODULE_PROPER})
		add_library(
			${EAGINE_MODULE_PROPER} STATIC
			$<TARGET_OBJECTS:${EAGINE_MODULE_OBJECTS}>)
		set_property(
			TARGET ${EAGINE_MODULE_PROPER}
			PROPERTY EAGINE_MODULE_PCM_PATH "${CMAKE_CURRENT_BINARY_DIR}")
		set_property(
			TARGET ${EAGINE_MODULE_PROPER}
			PROPERTY LINKER_LANGUAGE CXX)
		target_compile_options(
			${EAGINE_MODULE_PROPER}
			PUBLIC
				-fmodules
				-Wno-read-modules-implicitly
				"-fprebuilt-module-path=${CMAKE_CURRENT_BINARY_DIR}")
		install(
			TARGETS ${EAGINE_MODULE_PROPER}
			DESTINATION lib)
	endif()

	foreach(DIR ${EAGINE_MODULE_PRIVATE_INCLUDE_DIRECTORIES})
		target_include_directories(
			${EAGINE_MODULE_OBJECTS}
			PRIVATE "${DIR}")
	endforeach()

	foreach(LIB ${EAGINE_MODULE_PRIVATE_LINK_LIBRARIES})
		target_link_libraries(
			${EAGINE_MODULE_OBJECTS}
			PRIVATE "${LIB}")
		target_link_libraries(
			${EAGINE_MODULE_PROPER}
			PRIVATE "${LIB}")
	endforeach()

	foreach(NAME ${EAGINE_MODULE_SOURCES})
		configure_file(
			${NAME}_impl.cpp
			${EAGINE_MODULE_PROPER}-${NAME}_impl.cpp
			COPYONLY)
		target_sources(
			${EAGINE_MODULE_OBJECTS} PRIVATE
			"${CMAKE_CURRENT_BINARY_DIR}/${EAGINE_MODULE_PROPER}-${NAME}_impl.cpp")
	endforeach()

	set(EAGINE_MODULE_DEPENDS)
	set(EAGINE_MODULE_COMPILE_OPTIONS
		"-I${EAGINE_CORE_BINARY_ROOT}/include"
		"-fprebuilt-module-path=.")

	foreach(NAME ${EAGINE_MODULE_IMPORTS})
		if(TARGET ${NAME})
			get_property(
				PCM_PATH TARGET ${NAME}
				PROPERTY EAGINE_MODULE_PCM_PATH)

			if(NOT "${PCM_PATH}" STREQUAL "")
				list(
					APPEND EAGINE_MODULE_COMPILE_OPTIONS
					"-fprebuilt-module-path=${PCM_PATH}")

				if("${EAGINE_MODULE_PARTITION}" STREQUAL "")
					target_compile_options(
						${EAGINE_MODULE_OBJECTS}
						PUBLIC "-fprebuilt-module-path=${PCM_PATH}")
					target_link_libraries(
						${EAGINE_MODULE_OBJECTS}
						PUBLIC ${NAME})

					target_compile_options(
						${EAGINE_MODULE_PROPER}
						PUBLIC "-fprebuilt-module-path=${PCM_PATH}")
					target_link_libraries(
						${EAGINE_MODULE_PROPER}
						PUBLIC ${NAME})
				endif()
				list(APPEND EAGINE_MODULE_DEPENDS ${NAME})
			endif()
		elseif(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${NAME}.cppm")
			list(
				APPEND EAGINE_MODULE_DEPENDS
				${EAGINE_MODULE_PROPER}-${NAME}.pcm)
		endif()
	endforeach()

	foreach(NAME ${EAGINE_MODULE_SUBMODULES})
		get_property(
			PCM_PATH TARGET ${NAME}
			PROPERTY EAGINE_MODULE_PCM_PATH)
		if("${PCM_PATH}" STREQUAL "")
			message(FATAL_ERROR "Missing PCM path on ${NAME}")
		endif()

		list(
			APPEND EAGINE_MODULE_COMPILE_OPTIONS
			"-fprebuilt-module-path=${PCM_PATH}")

		if(NOT "${EAGINE_MODULE_PARTITION}" STREQUAL "")
			message(
				FATAL_ERROR
				"Partition ${EAGINE_MODULE_PARTITION} cannot have submodules")
		endif()

		target_compile_options(
			${EAGINE_MODULE_OBJECTS}
			PUBLIC "-fprebuilt-module-path=${PCM_PATH}")

		target_compile_options(
			${EAGINE_MODULE_PROPER}
			PUBLIC "-fprebuilt-module-path=${PCM_PATH}")
		target_link_libraries(
			${EAGINE_MODULE_PROPER}
			PRIVATE "${NAME}")
		target_sources(
			${EAGINE_MODULE_PROPER}
			PRIVATE $<TARGET_OBJECTS:${NAME}-objects>)

		get_property(
			COPIED_OBJECTS TARGET ${NAME}-objects
			PROPERTY SOURCES)
		list(FILTER COPIED_OBJECTS INCLUDE REGEX "^.*\\.o$")
		foreach(OBJECT ${COPIED_OBJECTS})
			set_source_files_properties(
				"${PCM_PATH}/${OBJECT}"
				PROPERTIES GENERATED 1)
			target_sources(
				${EAGINE_MODULE_PROPER}
				PRIVATE "${PCM_PATH}/${OBJECT}")
		endforeach()

		list(APPEND EAGINE_MODULE_DEPENDS ${NAME})
	endforeach()

	string(REPLACE "." "/" EAGINE_MODULE_DIRECTORY ${EAGINE_MODULE_PROPER})
	string(REGEX REPLACE "^eagine/" "" EAGINE_MODULE_DIRECTORY "${EAGINE_MODULE_DIRECTORY}")

	if("${EAGINE_MODULE_PARTITION}" STREQUAL "")
		string(REGEX REPLACE "^([a-z0-9_]+\\.)+([a-z0-9_]+)$" "\\2" SOURCE "${EAGINE_MODULE_PROPER}")
	else()
		set(SOURCE "${EAGINE_MODULE_PARTITION}")
	endif()

	if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE}.cpp")
		configure_file(${SOURCE}.cpp ${SOURCE}.cppm COPYONLY)
	elseif(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${SOURCE}.cppm")
		message(FATAL_ERROR "Missing source ${SOURCE}.cppm")
	endif()

	set_property(
		TARGET ${EAGINE_MODULE_PROPER}
		APPEND PROPERTY EAGINE_MODULE_INTERFACES
			"${CMAKE_CURRENT_BINARY_DIR}/${SOURCE}.cppm")
	if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
		install(
			FILES "${CMAKE_CURRENT_BINARY_DIR}/${SOURCE}.cppm"
			DESTINATION share/eagine/source/debug/${EAGINE_MODULE_DIRECTORY})
	endif()
	if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
		install(
			FILES "${CMAKE_CURRENT_BINARY_DIR}/${SOURCE}.cppm"
			DESTINATION share/eagine/source/release/${EAGINE_MODULE_DIRECTORY})
	endif()

	if("${EAGINE_MODULE_PARTITION}" STREQUAL "")
		set(OUTPUT ${EAGINE_MODULE_PROPER})
		get_property(
			PARTITIONS TARGET ${EAGINE_MODULE_PROPER}
			PROPERTY EAGINE_MODULE_PARTITIONS)
		foreach(PARTITION ${PARTITIONS})
			list(
				APPEND EAGINE_MODULE_DEPENDS
				"${EAGINE_MODULE_PROPER}-${PARTITION}.pcm")
		endforeach()
	else()
		set(OUTPUT ${EAGINE_MODULE_PROPER}-${EAGINE_MODULE_PARTITION})
		set_property(
			TARGET ${EAGINE_MODULE_PROPER}
			APPEND PROPERTY EAGINE_MODULE_PARTITIONS "${EAGINE_MODULE_PARTITION}")
	endif()

	list(REMOVE_DUPLICATES EAGINE_MODULE_DEPENDS)
	list(REMOVE_DUPLICATES EAGINE_MODULE_COMPILE_OPTIONS)

	add_custom_command(
		OUTPUT ${OUTPUT}.pcm
		COMMAND ${CMAKE_CXX_COMPILER}
		ARGS
			-std=c++${CMAKE_CXX_STANDARD}
			-fmodules
			-Wno-read-modules-implicitly
			${CMAKE_CXX_FLAGS}
			${EAGINE_MODULE_COMPILE_OPTIONS}
			${SOURCE}.cppm
			--precompile
			--output ${OUTPUT}.pcm
			DEPENDS ${EAGINE_MODULE_DEPENDS}
		COMMENT "Precompiling CXX module ${OUTPUT}")

	if("${EAGINE_MODULE_PARTITION}" STREQUAL "")
		set(EAGINE_MODULE_DEPENDS "${OUTPUT}.pcm")
	else()
		list(APPEND EAGINE_MODULE_DEPENDS "${OUTPUT}.pcm")
	endif()

	add_custom_command(
		OUTPUT ${OUTPUT}.o
		COMMAND ${CMAKE_CXX_COMPILER}
		ARGS
			-std=c++${CMAKE_CXX_STANDARD}
			-fmodules
			-Wno-read-modules-implicitly
			-Wno-unused-command-line-argument
			${CMAKE_CXX_FLAGS}
			${EAGINE_MODULE_COMPILE_OPTIONS}
			${OUTPUT}.pcm
			--compile
			--output ${OUTPUT}.o
		DEPENDS ${EAGINE_MODULE_DEPENDS}
		COMMENT "Compiling CXX object ${OUTPUT}")

	target_sources(${EAGINE_MODULE_OBJECTS} PRIVATE "${OUTPUT}.o")
	target_sources(${EAGINE_MODULE_PROPER} PRIVATE "${OUTPUT}.o")
endfunction()
# ------------------------------------------------------------------------------
function(eagine_target_modules TARGET_NAME)
	target_compile_options(
		${TARGET_NAME}
		PUBLIC
			-fmodules
			-Wno-read-modules-implicitly
	)
	foreach(EAGINE_MODULE ${ARGN})
		get_property(
			PCM_PATH TARGET ${EAGINE_MODULE}
			PROPERTY EAGINE_MODULE_PCM_PATH
		)
		target_compile_options(
			${TARGET_NAME}
			PUBLIC "-fprebuilt-module-path=${PCM_PATH}")
		target_link_libraries(${TARGET_NAME} PUBLIC ${EAGINE_MODULE})
	endforeach()
endfunction()
# ------------------------------------------------------------------------------
function(eagine_add_module_tests EAGINE_MODULE_PROPER)
	set(ARG_FLAGS)
	set(ARG_VALUES PARTITION)
	set(ARG_LISTS
		UNITS
		IMPORTS
		ENVIRONMENT
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
			foreach(ENV_VAR ${EAGINE_MODULE_TEST_ENVIRONMENT})
				if("${ENV_VAR}" MATCHES "^[A-Z_][A-Z0-9_]+=.*$")
					set_property(
						TEST "execute-${TEST_NAME}"
						APPEND PROPERTY
							ENVIRONMENT "${ENV_VAR}"
					)
				else()
					message(FATAL_ERROR "Invalid environment variable specification ${ENV_VAR}")
				endif()
			endforeach()
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
