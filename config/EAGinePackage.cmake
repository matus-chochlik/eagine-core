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

# General CPack options
set(CPACK_PACKAGE_NAME eagine)
set(CPACK_PACKAGE_VENDOR "EAGine")

set(CPACK_VERBATIM_VARIABLES YES)
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})
set(CPACK_OUTPUT_FILE_PREFIX
	"${PROJECT_BINARY_DIR}/eagine-${EAGINE_VERSION}-${EAGINE_GIT_COMMITS_SINCE_VERSION}")
set(CPACK_PACKAGE_VERSION "${EAGINE_VERSION}-${EAGINE_GIT_COMMITS_SINCE_VERSION}")
set(CPACK_PACKAGE_CONTACT "Matúš Chochlík <matus.chochlik@proton.me>")

set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_SOURCE eagine)
set(CPACK_DEBIAN_PACKAGE_PRIORITY "important")
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")

# Package specific options
#  Debian
#   Dependencies
set(CPACK_DEBIAN_CORE-TOOLS_PACKAGE_DEPENDS "coreutils,python3,python3-pip")
set(CPACK_DEBIAN_CORE-TOOLS_PACKAGE_SUGGESTS "postgresql-client,python3-psycopg2,python3-matplotlib")
#   Descriptions
set(CPACK_DEBIAN_USER-ACCOUNT_DESCRIPTION "Meta-package managing EAGine user account.")
set(CPACK_DEBIAN_CORE-TOOLS_DESCRIPTION "Collection of core command-line utilities for EAGine")

# Include CPack
set(CPACK_DEB_COMPONENT_INSTALL ON)
include(CPack)
