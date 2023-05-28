# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt

define_property(
	TARGET
	PROPERTY EAGINE_MODULE_PCM_PATH
	BRIEF_DOCS "Module PCM path"
	FULL_DOCS "Clang precompiled module path"
)
define_property(
	TARGET
	PROPERTY EAGINE_MODULE_INTERFACES
	BRIEF_DOCS "Module PCM path"
	FULL_DOCS "Clang precompiled module path"
)
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
		if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${EAGINE_MODULE_PARTITION}.cppm")
			list(APPEND EAGINE_MODULE_INTERFACES ${EAGINE_MODULE_PARTITION})
		elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${EAGINE_MODULE_PARTITION}.cpp")
			list(APPEND EAGINE_MODULE_INTERFACES ${EAGINE_MODULE_PARTITION})
		endif()
	endif()

	if(NOT TARGET ${EAGINE_MODULE_PROPER})
		add_library(${EAGINE_MODULE_PROPER} STATIC)
		set_property(
			TARGET ${EAGINE_MODULE_PROPER}
			PROPERTY LINKER_LANGUAGE CXX
		)
		set_property(
			TARGET ${EAGINE_MODULE_PROPER}
			PROPERTY EAGINE_MODULE_PCM_PATH "${CMAKE_CURRENT_BINARY_DIR}"
		)
		target_compile_options(
			${EAGINE_MODULE_PROPER}
			PUBLIC
				-Wno-read-modules-implicitly
				-fmodules
				"-fprebuilt-module-path=${CMAKE_CURRENT_BINARY_DIR}"
		)
		target_include_directories(
			${EAGINE_MODULE_PROPER}
			PRIVATE "${EAGINE_CORE_BINARY_ROOT}/include"
		)
	endif()

	foreach(DIR ${EAGINE_MODULE_PRIVATE_INCLUDE_DIRECTORIES})
		target_include_directories(
			${EAGINE_MODULE_PROPER}
			PRIVATE "${DIR}"
		)
	endforeach()

	foreach(LIB ${EAGINE_MODULE_PRIVATE_LINK_LIBRARIES})
		target_link_libraries(
			${EAGINE_MODULE_PROPER}
			PRIVATE "${LIB}"
		)
	endforeach()

	set(EAGINE_MODULE_OBJECT_DEPENDS)
	set(EAGINE_MODULE_OBJECT_OPTIONS
		"-I${EAGINE_CORE_BINARY_ROOT}/include"
		"-fprebuilt-module-path=.")

	foreach(NAME ${EAGINE_MODULE_IMPORTS})
		if(TARGET ${NAME})
			get_property(
				PCM_PATH TARGET ${NAME}
				PROPERTY EAGINE_MODULE_PCM_PATH
			)
			list(
				APPEND EAGINE_MODULE_OBJECT_DEPENDS
				${NAME}
			)
			if(NOT "${PCM_PATH}" STREQUAL "")
				list(
					APPEND EAGINE_MODULE_OBJECT_OPTIONS
					"-fprebuilt-module-path=${PCM_PATH}"
				)
				if("${EAGINE_MODULE_PARTITION}" STREQUAL "")
					target_compile_options(
						${EAGINE_MODULE_PROPER}
						PUBLIC "-fprebuilt-module-path=${PCM_PATH}"
					)
					target_link_libraries(${EAGINE_MODULE_PROPER} PUBLIC ${NAME})
				endif()
			endif()
		elseif(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${NAME}.cppm")
			list(
				APPEND EAGINE_MODULE_OBJECT_DEPENDS
				${EAGINE_MODULE_PROPER}-${NAME}.pcm
			)
		endif()
	endforeach()

	foreach(NAME ${EAGINE_MODULE_PARTITIONS})
		list(APPEND EAGINE_MODULE_OBJECT_DEPENDS ${EAGINE_MODULE_PROPER}-${NAME}.pcm)
	endforeach()

	foreach(NAME ${EAGINE_MODULE_SOURCES})
		target_sources(
			${EAGINE_MODULE_PROPER}
				PRIVATE "${NAME}_impl.cpp")
	endforeach()

	list(REMOVE_DUPLICATES EAGINE_MODULE_DEPENDS)
	list(REMOVE_DUPLICATES EAGINE_MODULE_OBJECT_OPTIONS)

	foreach(NAME ${EAGINE_MODULE_INTERFACES})
		if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.cpp")
			configure_file(${NAME}.cpp ${NAME}.cppm COPYONLY)
		endif()

		if("${EAGINE_MODULE_PARTITION}" STREQUAL "")
			set(EAGINE_MODULE_INTERFACE ${EAGINE_MODULE_PROPER})
		else()
			set(EAGINE_MODULE_INTERFACE ${EAGINE_MODULE_PROPER}-${NAME})
		endif()

		set_property(
			TARGET ${EAGINE_MODULE_PROPER}
			APPEND PROPERTY EAGINE_MODULE_INTERFACES "${NAME}.cppm"
		)

		add_custom_command(
			OUTPUT ${EAGINE_MODULE_INTERFACE}.pcm
			COMMAND ${CMAKE_CXX_COMPILER}
			ARGS
				-std=c++${CMAKE_CXX_STANDARD}
				-fmodules
				${CMAKE_CXX_FLAGS}
				${EAGINE_MODULE_OBJECT_OPTIONS}
				-Wno-read-modules-implicitly
				${NAME}.cppm
				--precompile
				--output ${EAGINE_MODULE_INTERFACE}.pcm
			DEPENDS ${EAGINE_MODULE_OBJECT_DEPENDS}
			COMMENT "Precompiling CXX module ${EAGINE_MODULE_INTERFACE}"
		)

		add_custom_command(
			OUTPUT ${EAGINE_MODULE_INTERFACE}.o
			COMMAND ${CMAKE_CXX_COMPILER}
			ARGS
				-std=c++${CMAKE_CXX_STANDARD}
				-fmodules
				${CMAKE_CXX_FLAGS}
				${EAGINE_MODULE_OBJECT_OPTIONS}
				-Wno-read-modules-implicitly
				-Wno-unused-command-line-argument
				${EAGINE_MODULE_INTERFACE}.pcm
				--compile
				--output ${EAGINE_MODULE_INTERFACE}.o
			DEPENDS ${EAGINE_MODULE_OBJECT_DEPENDS};${EAGINE_MODULE_INTERFACE}.pcm
			COMMENT "Compiling CXX object ${EAGINE_MODULE_INTERFACE}"
		)

		target_sources(
			${EAGINE_MODULE_PROPER}
				PRIVATE "${EAGINE_MODULE_INTERFACE}.o"
		)
	endforeach()
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
