# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt

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
define_property(
	TARGET
	PROPERTY EAGINE_MODULE_SUBMODULES
	BRIEF_DOCS "Module sub-module list"
	FULL_DOCS "List of submodule names for a module")
define_property(
	TARGET
	PROPERTY EAGINE_MODULE_BUILD_DIR
	BRIEF_DOCS "Module build directory"
	FULL_DOCS "Path to the build directory for a module")
# ------------------------------------------------------------------------------
function(eagine_add_module EAGINE_MODULE_PROPER)
	set(ARG_FLAGS)
	set(ARG_VALUES
		COMPONENT
		PARTITION)
	set(ARG_LISTS
		SOURCES
		IMPORTS
		SUBMODULES
		PRIVATE_INCLUDE_DIRECTORIES
		PRIVATE_LINK_LIBRARIES
		INTERFACE_LINK_LIBRARIES)
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

	set(EAGINE_MODULE_CMAKE_FILE
		"${CMAKE_CURRENT_BINARY_DIR}/module-${EAGINE_MODULE_PROPER}.cmake")

	if(NOT TARGET ${EAGINE_MODULE_PROPER})
		add_library(
			${EAGINE_MODULE_PROPER} STATIC
			$<TARGET_OBJECTS:${EAGINE_MODULE_OBJECTS}>)
		set_property(
			TARGET ${EAGINE_MODULE_PROPER}
			PROPERTY EAGINE_MODULE_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}")
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
		file(
			WRITE "${EAGINE_MODULE_CMAKE_FILE}"
			"# Generated file. All manual changes will be lost!\n"
			"#\n"
			"file(MAKE_DIRECTORY \"\${PROJECT_BINARY_DIR}/EAGine\")\n")
		install(
			FILES "${EAGINE_MODULE_CMAKE_FILE}"
			COMPONENT ${EAGINE_MODULE_COMPONENT}
			DESTINATION "${EAGINE_CMAKE_CONFIG_DEST}/${CMAKE_BUILD_TYPE}")
		install(
			TARGETS ${EAGINE_MODULE_PROPER}
			COMPONENT ${EAGINE_MODULE_COMPONENT}
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
		add_custom_command(
			OUTPUT ${EAGINE_MODULE_PROPER}-${NAME}_impl.cpp
			COMMAND ${CMAKE_COMMAND}
			ARGS
				-E copy_if_different
				"${CMAKE_CURRENT_SOURCE_DIR}/${NAME}_impl.cpp"
				"${CMAKE_CURRENT_BINARY_DIR}/${EAGINE_MODULE_PROPER}-${NAME}_impl.cpp"
			DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${NAME}_impl.cpp"
			COMMENT "Copying CXX module implementation ${NAME}_impl.cpp")
		target_sources(
			${EAGINE_MODULE_OBJECTS} PRIVATE
			"${CMAKE_CURRENT_BINARY_DIR}/${EAGINE_MODULE_PROPER}-${NAME}_impl.cpp")
	endforeach()

	set(EAGINE_MODULE_DEPENDS)
	set(EAGINE_MODULE_IMPORT_PCM_DEPENDS)
	set(EAGINE_MODULE_COMPILE_OPTIONS
		"-I${EAGINE_CORE_BINARY_ROOT}/include"
		"-fprebuilt-module-path=.")

	foreach(DIR ${EAGINE_MODULE_PRIVATE_INCLUDE_DIRECTORIES})
		list(APPEND EAGINE_MODULE_COMPILE_OPTIONS "-I${DIR}")
	endforeach()

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
				list(
					APPEND EAGINE_MODULE_IMPORT_PCM_DEPENDS
					"\${PROJECT_BINARY_DIR}/EAGine/${NAME}.pcm")
			endif()
			file(
				APPEND "${EAGINE_MODULE_CMAKE_FILE}"
				"if(NOT TARGET ${NAME})\n"
				"	include(\"\${_IMPORT_PREFIX}/${EAGINE_CMAKE_CONFIG_DEST}/${CMAKE_BUILD_TYPE}/module-${NAME}.cmake\")\n"
				"endif()\n")
		elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.cpp")
			list(
				APPEND EAGINE_MODULE_DEPENDS
				${EAGINE_MODULE_PROPER}-${NAME}.pcm)
			list(
				APPEND EAGINE_MODULE_IMPORT_PCM_DEPENDS
				"\${PROJECT_BINARY_DIR}/EAGine/${EAGINE_MODULE_PROPER}-${NAME}.pcm")
		endif()
	endforeach()

	foreach(NAME ${EAGINE_MODULE_SUBMODULES})
		set_property(
			TARGET ${EAGINE_MODULE_PROPER}
			APPEND PROPERTY EAGINE_MODULE_SUBMODULES
				${NAME}
		)
		get_property(
			PCM_PATH TARGET ${EAGINE_MODULE_PROPER}.${NAME}
			PROPERTY EAGINE_MODULE_PCM_PATH)
		if("${PCM_PATH}" STREQUAL "")
			message(FATAL_ERROR "Missing PCM path on ${EAGINE_MODULE_PROPER}.${NAME}")
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
			PRIVATE "${EAGINE_MODULE_PROPER}.${NAME}")
		target_sources(
			${EAGINE_MODULE_PROPER}
			PRIVATE $<TARGET_OBJECTS:${EAGINE_MODULE_PROPER}.${NAME}-objects>)

		get_property(
			COPIED_OBJECTS TARGET ${EAGINE_MODULE_PROPER}.${NAME}-objects
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

		list(APPEND EAGINE_MODULE_DEPENDS ${EAGINE_MODULE_PROPER}.${NAME})
		list(
			APPEND EAGINE_MODULE_IMPORT_PCM_DEPENDS
			"\${PROJECT_BINARY_DIR}/EAGine/${EAGINE_MODULE_PROPER}.${NAME}.pcm")
		file(
			APPEND "${EAGINE_MODULE_CMAKE_FILE}"
			"if(NOT TARGET ${EAGINE_MODULE_PROPER}.${NAME})\n"
			"	include(\"\${_IMPORT_PREFIX}/${EAGINE_CMAKE_CONFIG_DEST}/${CMAKE_BUILD_TYPE}/module-${EAGINE_MODULE_PROPER}.${NAME}.cmake\")\n"
			"endif()\n")
	endforeach()

	string(REPLACE "." "/" EAGINE_MODULE_DIRECTORY ${EAGINE_MODULE_PROPER})
	string(REGEX REPLACE "^eagine/" "" EAGINE_MODULE_DIRECTORY "${EAGINE_MODULE_DIRECTORY}")

	if("${EAGINE_MODULE_PARTITION}" STREQUAL "")
		string(REGEX REPLACE "^([a-z0-9_]+\\.)+([a-z0-9_]+)$" "\\2" SOURCE "${EAGINE_MODULE_PROPER}")
	else()
		set(SOURCE "${EAGINE_MODULE_PARTITION}")
	endif()

	if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE}.cpp")
		add_custom_command(
			OUTPUT ${SOURCE}.cppm
			COMMAND ${CMAKE_CXX_COMPILER}
			ARGS
				-std=c++${CMAKE_CXX_STANDARD}
				-fmodules
				-Wno-read-modules-implicitly
				${CMAKE_CXX_FLAGS}
				${EAGINE_MODULE_COMPILE_OPTIONS}
				"${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE}.cpp"
				--preprocess
				--output ${SOURCE}.cppm
			DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE}.cpp"
			COMMENT "Preprocessing CXX module interface ${SOURCE}.cpp")
	elseif(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${SOURCE}.cppm")
		message(FATAL_ERROR "Missing source ${SOURCE}.cppm")
	endif()

	set_property(
		TARGET ${EAGINE_MODULE_PROPER}
		APPEND PROPERTY EAGINE_MODULE_INTERFACES
			"${CMAKE_CURRENT_BINARY_DIR}/${SOURCE}.cppm")
	install(
		FILES "${CMAKE_CURRENT_BINARY_DIR}/${SOURCE}.cppm"
		COMPONENT ${EAGINE_MODULE_COMPONENT}
		DESTINATION share/eagine/source/${CMAKE_BUILD_TYPE}/${EAGINE_MODULE_DIRECTORY})

	if("${EAGINE_MODULE_PARTITION}" STREQUAL "")
		set(OUTPUT ${EAGINE_MODULE_PROPER})
		get_property(
			PARTITIONS TARGET ${EAGINE_MODULE_PROPER}
			PROPERTY EAGINE_MODULE_PARTITIONS)
		foreach(PARTITION ${PARTITIONS})
			list(
				APPEND EAGINE_MODULE_DEPENDS
				"${EAGINE_MODULE_PROPER}-${PARTITION}.pcm")
			list(
				APPEND EAGINE_MODULE_IMPORT_PCM_DEPENDS
				"\${PROJECT_BINARY_DIR}/EAGine/${EAGINE_MODULE_PROPER}-${PARTITION}.pcm")
		endforeach()
	else()
		set(OUTPUT ${EAGINE_MODULE_PROPER}-${EAGINE_MODULE_PARTITION})
		set_property(
			TARGET ${EAGINE_MODULE_PROPER}
			APPEND PROPERTY EAGINE_MODULE_PARTITIONS "${EAGINE_MODULE_PARTITION}")
	endif()

	list(REMOVE_DUPLICATES EAGINE_MODULE_DEPENDS)
	list(REMOVE_DUPLICATES EAGINE_MODULE_IMPORT_PCM_DEPENDS)
	list(REMOVE_DUPLICATES EAGINE_MODULE_COMPILE_OPTIONS)

	set(EAGINE_MODULE_PCM_DEPENDS "${SOURCE}.cppm")
	if(NOT "${EAGINE_MODULE_DEPENDS}" STREQUAL "")
		list(APPEND EAGINE_MODULE_PCM_DEPENDS "${EAGINE_MODULE_DEPENDS}")
	endif()

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
		DEPENDS ${EAGINE_MODULE_PCM_DEPENDS}
		COMMENT "Precompiling CXX module ${OUTPUT}")

	file(
		APPEND "${EAGINE_MODULE_CMAKE_FILE}"
		"add_custom_command(\n"
		"	OUTPUT \"\${PROJECT_BINARY_DIR}/EAGine/${OUTPUT}.pcm\"\n"
		"	COMMAND \${CMAKE_CXX_COMPILER}\n"
		"	ARGS\n"
		"		-std=c++\${CMAKE_CXX_STANDARD}\n"
		"		-fmodules\n"
		"		-Wno-read-modules-implicitly\n"
		"		\"-fprebuilt-module-path=\${PROJECT_BINARY_DIR}/EAGine\"\n"
		"		\${CMAKE_CXX_FLAGS}\n"
		"		\"\${_IMPORT_PREFIX}/share/eagine/source/${CMAKE_BUILD_TYPE}/${EAGINE_MODULE_DIRECTORY}/${SOURCE}.cppm\"\n"
		"		--precompile\n"
		"		--output \"\${PROJECT_BINARY_DIR}/EAGine/${OUTPUT}.pcm\"\n")
	if(NOT "${EAGINE_MODULE_IMPORT_PCM_DEPENDS}" STREQUAL "")
		file(
			APPEND "${EAGINE_MODULE_CMAKE_FILE}"
			"	DEPENDS \"${EAGINE_MODULE_IMPORT_PCM_DEPENDS}\"\n")
	endif()
	file(
		APPEND "${EAGINE_MODULE_CMAKE_FILE}"
		"	COMMENT \"Precompiling CXX module ${OUTPUT}.pcm\")\n")

	if("${EAGINE_MODULE_PARTITION}" STREQUAL "")
		if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
			set(CONFIG_POSTFIX "${CMAKE_RELEASE_POSTFIX}")
		elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
			set(CONFIG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
		endif()
		set(EAGINE_MODULE_DEPENDS "${OUTPUT}.pcm")
		file(
			APPEND "${EAGINE_MODULE_CMAKE_FILE}"
			"add_custom_target(${EAGINE_MODULE_PROPER}-pcm DEPENDS \"\${PROJECT_BINARY_DIR}/EAGine/${EAGINE_MODULE_PROPER}.pcm\")\n"
			"add_library(${EAGINE_MODULE_PROPER} STATIC IMPORTED GLOBAL)\n"
			"add_dependencies(${EAGINE_MODULE_PROPER} ${EAGINE_MODULE_PROPER}-pcm)\n"
			"set_target_properties(\n"
			"	${EAGINE_MODULE_PROPER} PROPERTIES\n"
			"	IMPORTED_LOCATION \"\${_IMPORT_PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}${EAGINE_MODULE_PROPER}${CONFIG_POSTFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}\")\n"
			"set_target_properties(\n"
			"	${EAGINE_MODULE_PROPER} PROPERTIES\n"
			"	INTERFACE_COMPILE_OPTIONS\n" 
			"		\"-fmodules;-Wno-read-modules-implicitly;-fmodule-file=${EAGINE_MODULE_PROPER}=\${PROJECT_BINARY_DIR}/EAGine/${EAGINE_MODULE_PROPER}.pcm\"\n"
			"	INTERFACE_LINK_DIRECTORIES\n" 
			"		\"\${_IMPORT_PREFIX}/lib\"\n")
		if(NOT "${EAGINE_MODULE_INTERFACE_LINK_LIBRARIES}" STREQUAL "")
			unset(INTERFACE_LIBS)
			foreach(LIB ${EAGINE_MODULE_INTERFACE_LINK_LIBRARIES})
				if(TARGET ${LIB})
					list(APPEND INTERFACE_LIBS "${LIB}${CONFIG_POSTFIX}")
				else()
					list(APPEND INTERFACE_LIBS ${LIB})
				endif()
			endforeach()
			if(NOT "${INTERFACE_LIBS}" STREQUAL "")
			file(
				APPEND "${EAGINE_MODULE_CMAKE_FILE}"
				"	INTERFACE_LINK_LIBRARIES\n"
				"		\"${INTERFACE_LIBS}\"\n")
			endif()
		endif()
		file(
			APPEND "${EAGINE_MODULE_CMAKE_FILE}"
			")\n"
			"file(GLOB _ADDITIONAL_INCLUDES \"\${_IMPORT_PREFIX}/${EAGINE_CMAKE_CONFIG_DEST}/${CMAKE_BUILD_TYPE}/module-${EAGINE_MODULE_PROPER}-*.cmake\")\n"
			"foreach(INCL \${_ADDITIONAL_INCLUDES})\n"
			"	include(\"\${INCL}\")\n"
			"endforeach()\n")
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
		if(NOT "${EAGINE_MODULE}" STREQUAL "std")
			get_property(
				PCM_PATH TARGET ${EAGINE_MODULE}
				PROPERTY EAGINE_MODULE_PCM_PATH
			)
			target_compile_options(
				${TARGET_NAME}
				PUBLIC "-fprebuilt-module-path=${PCM_PATH}")
			target_link_libraries(${TARGET_NAME} PUBLIC ${EAGINE_MODULE})
		endif()
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
		PRIVATE_LINK_LIBRARIES
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
			list(APPEND EAGINE_MODULE_TEST_PRIVATE_LINK_LIBRARIES
				eagine-core-testing-headers)
			target_link_libraries(
				${TEST_NAME}
				PRIVATE ${EAGINE_MODULE_TEST_PRIVATE_LINK_LIBRARIES}
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
# ------------------------------------------------------------------------------
function(eagine_do_generate_module_json PREFIX OUTPUT MODULE_NAME)
	unset(FULL_MODULE_NAME)
	unset(FULL_MODULE_PATH)
	foreach(ENTRY ${ARGN})
		set(FULL_MODULE_NAME "${FULL_MODULE_NAME}${ENTRY}.")
		set(FULL_MODULE_PATH "${FULL_MODULE_PATH}${ENTRY}/")
	endforeach()
	set(FULL_MODULE_NAME "${FULL_MODULE_NAME}${MODULE_NAME}")
	set(FULL_MODULE_PATH "${FULL_MODULE_PATH}${MODULE_PATH}")

	file(APPEND "${OUTPUT}" "{\"name\":\"${MODULE_NAME}\"")
	file(APPEND "${OUTPUT}" ",\"full_name\":\"${FULL_MODULE_NAME}\"")

	get_property(
		MODULE_BUILD_DIR TARGET ${FULL_MODULE_NAME}
		PROPERTY EAGINE_MODULE_BUILD_DIR)

	file(RELATIVE_PATH MODULE_BUILD_DIR "${PREFIX}" "${MODULE_BUILD_DIR}")

	file(APPEND "${OUTPUT}" ",\"libraries\":[\"")
	if(MODULE_BUILD_DIR)
		file(APPEND "${OUTPUT}" "${MODULE_BUILD_DIR}/")
	endif()
	file(APPEND "${OUTPUT}" "\$<TARGET_FILE_NAME:${FULL_MODULE_NAME}>\"]")

	file(APPEND "${OUTPUT}" ",\"cmakefiles\":[\"")
	if(MODULE_BUILD_DIR)
		file(APPEND "${OUTPUT}" "${MODULE_BUILD_DIR}/")
	endif()
	file(APPEND "${OUTPUT}" "module-${FULL_MODULE_NAME}.cmake\"]")

	file(APPEND "${OUTPUT}" ",\"interfaces\":[")
	get_property(
		MODULE_CPPMS TARGET ${FULL_MODULE_NAME}
		PROPERTY EAGINE_MODULE_INTERFACES)
	set(FIRST_CPPM TRUE)
	foreach(CPPM_PATH ${MODULE_CPPMS})
		if(FIRST_CPPM)
			set(FIRST_CPPM FALSE)
		else()
			file(APPEND "${OUTPUT}" ",")
		endif()
		file(RELATIVE_PATH CPPM_PATH "${PREFIX}" "${CPPM_PATH}")
		file(APPEND "${OUTPUT}" "\"${CPPM_PATH}\"")
	endforeach()
	file(APPEND "${OUTPUT}" "]")

	file(APPEND "${OUTPUT}" ",\"submodules\":[")
	get_property(
		MODULE_SUBMODULES TARGET ${FULL_MODULE_NAME}
		PROPERTY EAGINE_MODULE_SUBMODULES)
	set(FIRST_SUBMOD TRUE)
	foreach(SUBMODULE ${MODULE_SUBMODULES})
		if(FIRST_SUBMOD)
			set(FIRST_SUBMOD FALSE)
		else()
			file(APPEND "${OUTPUT}" ",")
		endif()
		eagine_do_generate_module_json(
			"${PREFIX}" "${OUTPUT}"
			${SUBMODULE} ${ARGN} ${MODULE_NAME})
	endforeach()
	file(APPEND "${OUTPUT}" "]")

	file(APPEND "${OUTPUT}" "}")
endfunction()
# ------------------------------------------------------------------------------
function(eagine_generate_module_json MODULE_NAME)
	get_property(
		MODULE_BUILD_DIR TARGET ${MODULE_NAME}
		PROPERTY EAGINE_MODULE_BUILD_DIR)

	file(
		WRITE "${CMAKE_CURRENT_BINARY_DIR}/${MODULE_NAME}.json.in"
		"{\"build_directory\":\"${MODULE_BUILD_DIR}\"")
	file(
		APPEND "${CMAKE_CURRENT_BINARY_DIR}/${MODULE_NAME}.json.in"
		",\"module\":")

	eagine_do_generate_module_json(
		"${MODULE_BUILD_DIR}"
		"${CMAKE_CURRENT_BINARY_DIR}/${MODULE_NAME}.json.in"
		${MODULE_NAME} ${ARGS})

	file(
		APPEND "${CMAKE_CURRENT_BINARY_DIR}/${MODULE_NAME}.json.in"
		"}")

	file(
		GENERATE
		OUTPUT "${MODULE_NAME}.json"
		INPUT "${CMAKE_CURRENT_BINARY_DIR}/${MODULE_NAME}.json.in")
endfunction()
# ------------------------------------------------------------------------------
