#  Copyright Matus Chochlik.
#  Distributed under the Boost Software License, Version 1.0.
#  See accompanying file LICENSE_1_0.txt or copy at
#   http://www.boost.org/LICENSE_1_0.txt
#
include(FindPkgConfig)

macro(eagine_common_import_lib)
	cmake_parse_arguments(
		EAGINE_DEPS
		""
		"PREFIX;PKGCONFIG"
		"HEADER;LIBRARY"
		${ARGN}
	)

	unset(${EAGINE_DEPS_PREFIX}_FOUND)
	unset(${EAGINE_DEPS_PREFIX}_DEFINITIONS)
	unset(${EAGINE_DEPS_PREFIX}_INCLUDE_DIRS)
	unset(${EAGINE_DEPS_PREFIX}_LIBRARY_DIRS)
	unset(${EAGINE_DEPS_PREFIX}_LIBRARIES)

	if(PKG_CONFIG_FOUND)
		pkg_check_modules(PC_${EAGINE_DEPS_PREFIX} QUIET ${EAGINE_DEPS_PKGCONFIG})
		if(PC_${EAGINE_DEPS_PREFIX}_FOUND)
			add_library(EAGine::Deps::${EAGINE_DEPS_PREFIX} INTERFACE IMPORTED)

			if(PC_${EAGINE_DEPS_PREFIX}_CFLAGS_OTHER)
				target_compile_options(
					EAGine::Deps::${EAGINE_DEPS_PREFIX}
					INTERFACE "${PC_${EAGINE_DEPS_PREFIX}_CFLAGS_OTHER}"
				)
			endif()
			if(PC_${EAGINE_DEPS_PREFIX}_INCLUDE_DIRS)
				target_include_directories(
					EAGine::Deps::${EAGINE_DEPS_PREFIX}
					INTERFACE "${PC_${EAGINE_DEPS_PREFIX}_INCLUDE_DIRS}"
				)
			endif()
			if(PC_${EAGINE_DEPS_PREFIX}_LDFLAGS_OTHER)
				set_target_properties(
					EAGine::Deps::${EAGINE_DEPS_PREFIX} PROPERTIES
					INTERFACE_LINK_OPTIONS
					"${PC_${EAGINE_DEPS_PREFIX}_LDFLAGS_OTHER}"
				)
			endif()
			if(PC_${EAGINE_DEPS_PREFIX}_LIBRARY_DIRS)
				set_target_properties(
					EAGine::Deps::${EAGINE_DEPS_PREFIX} PROPERTIES
					INTERFACE_LINK_DIRECTORIES
					"${PC_${EAGINE_DEPS_PREFIX}_LIBRARY_DIRS}"
				)
			endif()
			if(PC_${EAGINE_DEPS_PREFIX}_LIBRARIES)
				target_link_libraries(
					EAGine::Deps::${EAGINE_DEPS_PREFIX}
					INTERFACE "${PC_${EAGINE_DEPS_PREFIX}_LIBRARIES}"
				)
			endif()
			message(STATUS
				"Found ${EAGINE_DEPS_PREFIX}: "
				"${PC_${EAGINE_DEPS_PREFIX}_INCLUDE_DIRS} "
				"${PC_${EAGINE_DEPS_PREFIX}_LIBRARIES}"
			)
			set(${EAGINE_DEPS_PREFIX}_FOUND 1)
		endif()
		unset(PC_${EAGINE_DEPS_PREFIX}_CFLAGS)
		unset(PC_${EAGINE_DEPS_PREFIX}_CFLAGS_OTHER)
		unset(PC_${EAGINE_DEPS_PREFIX}_INCLUDE_DIRS)
		unset(PC_${EAGINE_DEPS_PREFIX}_LDFLAGS)
		unset(PC_${EAGINE_DEPS_PREFIX}_LDFLAGS_OTHER)
		unset(PC_${EAGINE_DEPS_PREFIX}_LIBRARY_DIRS)
		unset(PC_${EAGINE_DEPS_PREFIX}_LIBRARIES)
	endif()

	if(NOT ${EAGINE_DEPS_PREFIX}_FOUND)
		# TODO: try manually
	endif()

	if(NOT ${EAGINE_DEPS_PREFIX}_FOUND)
		set(${EAGINE_DEPS_PREFIX}_FOUND 0)
	endif()

	unset(${EAGINE_DEPS_PREFIX}_DEFINITIONS)
	unset(${EAGINE_DEPS_PREFIX}_INCLUDE_DIRS)
	unset(${EAGINE_DEPS_PREFIX}_LIBRARY_DIRS)
	unset(${EAGINE_DEPS_PREFIX}_LIBRARIES)
endmacro()
