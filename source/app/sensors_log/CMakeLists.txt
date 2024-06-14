# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt
add_executable(eagine-core-sensors-log main.cpp)

eagine_add_exe_analysis(eagine-core-sensors-log)
eagine_target_modules(
	eagine-core-sensors-log
	std
	eagine.core)

eagine_embed_target_resources(
	TARGET eagine-core-sensors-log
	RESOURCES
		BoostLcnse "../../../LICENSE_1_0.txt"
		Manual     "resource/eagine-core-sensors-log.man"
		BashCmpltn "resource/eagine-core-sensors-log.bco"
	PACKED
	ENABLE_SEARCH)

install(
	TARGETS eagine-core-sensors-log
	COMPONENT core-logging
	RUNTIME DESTINATION bin)

add_custom_command(
	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/eagine-core-sensors-log.1.gz"
	COMMAND ${GZIP_COMMAND} -cn9
				"${CMAKE_CURRENT_SOURCE_DIR}/resource/eagine-core-sensors-log.man"
				> "${CMAKE_CURRENT_BINARY_DIR}/eagine-core-sensors-log.1.gz"
	DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/resource/eagine-core-sensors-log.man")

add_custom_target("eagine-core-sensors-log-man"
	ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/eagine-core-sensors-log.1.gz")

install(
	FILES "${CMAKE_CURRENT_BINARY_DIR}/eagine-core-sensors-log.1.gz"
	COMPONENT core-logging
	PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
	DESTINATION share/man/man1)

install(
	FILES "resource/eagine-core-sensors-log.bco"
	COMPONENT core-logging
	RENAME eagine-core-sensors-log
	PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
	DESTINATION share/bash-completion/completions)

install(
	DIRECTORY etc
	COMPONENT core-logging
	DESTINATION /)

