#  Copyright Matus Chochlik.
#  Distributed under the Boost Software License, Version 1.0.
#  See accompanying file LICENSE_1_0.txt or copy at
#  https://www.boost.org/LICENSE_1_0.txt
#
find_program(GZIP_COMMAND gzip)

function(eagine_add_license COMPONENT)
	install(
		FILES "${PROJECT_SOURCE_DIR}/LICENSE_1_0.txt"
		COMPONENT ${COMPONENT}
		RENAME copyright
		PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
		DESTINATION /usr/share/doc/eagine-${COMPONENT})
endfunction()

function(eagine_add_debian_changelog COMPONENT)
	if(GZIP_COMMAND)
		add_custom_command(
			OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/changelog.gz"
			COMMAND ${GZIP_COMMAND} -cn9 "${CMAKE_CURRENT_SOURCE_DIR}/deb-changelog"
						> "${CMAKE_CURRENT_BINARY_DIR}/changelog.gz"
			DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/deb-changelog")

		add_custom_target("${COMPONENT}-deb-changelog"
			ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/changelog.gz")

		install(
			FILES "${CMAKE_CURRENT_BINARY_DIR}/changelog.gz"
			COMPONENT ${COMPONENT}
			PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
			DESTINATION /usr/share/doc/eagine-${COMPONENT})
	endif()
endfunction()

set(EAGINE_CPACK_PROPS "${CMAKE_BINARY_DIR}/CPackPropertiesEAGine.cmake")

function(eagine_add_package_property COMPONENT)
	set(ARG_FLAGS)
	set(ARG_VALUES GENERATOR VARIABLE)
	set(ARG_LISTS VALUE)
	string(TOUPPER "${COMPONENT}" COMPONENT_UC)
	cmake_parse_arguments(
		EAGINE_PACKAGE
		"${ARG_FLAGS}" "${ARG_VALUES}" "${ARG_LISTS}"
		${ARGN})
	if(NOT "${EAGINE_PACKAGE_VARIABLE}" STREQUAL "PACKAGE_CONTROL_EXTRA")
		file(APPEND "${EAGINE_CPACK_PROPS}"
			"set(CPACK_${EAGINE_PACKAGE_GENERATOR}_${COMPONENT_UC}_${EAGINE_PACKAGE_VARIABLE} \"")
	endif()
	set(FIRST TRUE)
	foreach(VALUE ${EAGINE_PACKAGE_VALUE})
		if("${EAGINE_PACKAGE_VARIABLE}" STREQUAL "PACKAGE_CONTROL_EXTRA")
			if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${VALUE}")
				set(PROP_VALUE "${CMAKE_CURRENT_BINARY_DIR}/${VALUE}")
			elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${VALUE}")
				set(PROP_VALUE "${CMAKE_CURRENT_SOURCE_DIR}/${VALUE}")
			else()
				set(PROP_VALUE "${VALUE}")
			endif()
			file(APPEND "${EAGINE_CPACK_PROPS}"
				"list(APPEND CPACK_${EAGINE_PACKAGE_GENERATOR}_${COMPONENT_UC}_${EAGINE_PACKAGE_VARIABLE} \"${PROP_VALUE}\")\n")
		else()
			if(FIRST)
				set(FIRST FALSE)
			else()
				file(APPEND "${EAGINE_CPACK_PROPS}" ";")
			endif()
			file(APPEND "${EAGINE_CPACK_PROPS}" "${VALUE}")
		endif()
	endforeach()
	if(NOT "${EAGINE_PACKAGE_VARIABLE}" STREQUAL "PACKAGE_CONTROL_EXTRA")
		file(APPEND "${EAGINE_CPACK_PROPS}" "\")\n")
	endif()
endfunction()

function(eagine_add_package_conffiles COMPONENT)
	string(TOUPPER "${COMPONENT}" COMPONENT_UC)
	foreach(ARG ${ARGN})
		if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${ARGN}")
			set(EAGINE_CONFFILE_PREFIX "") # TODO parse args
			if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${ARGN}")
				file(GLOB_RECURSE
					CONFFILES
					LIST_DIRECTORIES FALSE
					RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
					"${CMAKE_CURRENT_SOURCE_DIR}/${ARGN}/*")
				foreach(CONFFILE ${CONFFILES})
					file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/${COMPONENT}")
					file(APPEND "${CMAKE_BINARY_DIR}/${COMPONENT}/conffiles"
						"${EAGINE_CONFFILE_PREFIX}/${CONFFILE}\n")
				endforeach()

			endif()
		endif()
	endforeach()
	if(EXISTS "${CMAKE_BINARY_DIR}/${COMPONENT}/conffiles")
		file(APPEND "${EAGINE_CPACK_PROPS}"
			"list(APPEND CPACK_DEBIAN_${COMPONENT_UC}_PACKAGE_CONTROL_EXTRA \"${CMAKE_BINARY_DIR}/${COMPONENT}/conffiles\")\n")
	endif()
endfunction()

if(NOT EXISTS "${EAGINE_CPACK_PROPS}")
	file(WRITE "${EAGINE_CPACK_PROPS}"
		"set(CXX_RUNTIME_PKGS \"libc6,libc++1-17\")\n"
		"set(EAGINE_CORE_RUNTIME_PKGS \"libsystemd0,zlib1g\")\n")
endif()

# General CPack options
set(CPACK_PACKAGE_NAME eagine)
set(CPACK_PACKAGE_VENDOR "EAGine")

set(CPACK_VERBATIM_VARIABLES YES)
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})
set(CPACK_OUTPUT_FILE_PREFIX "${PROJECT_BINARY_DIR}/release")
set(CPACK_PACKAGE_VERSION "${EAGINE_VERSION}-${EAGINE_GIT_COMMITS_SINCE_VERSION}")
set(CPACK_PACKAGE_CONTACT "Matúš Chochlík <matus.chochlik@proton.me>")

set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_SOURCE eagine)
set(CPACK_DEBIAN_PACKAGE_PRIORITY "medium")
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
set(CPACK_DEBIAN_PACKAGE_CONTROL_STRICT_PERMISSION TRUE)

# Include CPack
set(CPACK_COMPONENT_UNSPECIFIED_HIDDEN TRUE)
set(CPACK_DEB_COMPONENT_INSTALL TRUE)
set(CPACK_PROPERTIES_FILE "${EAGINE_CPACK_PROPS}")

include(CPack)
