# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt

foreach(APP git-filter fmd-modify fmd-get psql-logs-cl log-plot procman)
	add_custom_command(
		OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/eagine-${APP}.bco"
		COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/${APP}.py"
					--print-bash-completion
					"${CMAKE_CURRENT_BINARY_DIR}/eagine-${APP}.bco"
		DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${APP}.py")

	add_custom_target("eagine-${APP}-bco"
		ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/eagine-${APP}.bco")

	install(
		FILES "${CMAKE_CURRENT_BINARY_DIR}/eagine-${APP}.bco"
		COMPONENT core-tools
		RENAME eagine-${APP}
		PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
		DESTINATION /usr/share/bash-completion/completions)

	install(
		PROGRAMS ${APP}.py
		COMPONENT core-tools
		RENAME eagine-${APP}
		DESTINATION bin)
endforeach()

install(
	PROGRAMS ensure-main-key.sh
	COMPONENT core-tools
	RENAME eagine-ensure-main-key
	DESTINATION bin)
install(
	PROGRAMS log-schema-stats.sh
	COMPONENT core-tools
	RENAME eagine-log-schema-stats
	DESTINATION bin)
install(
	PROGRAMS log-psql.sh
	COMPONENT core-tools
	RENAME eagilog-psql
	DESTINATION bin)

add_subdirectory(log_server)
add_subdirectory(sensors_log)

find_package(Qt5 COMPONENTS Core Widgets Xml Qml Quick)
if(Qt5_FOUND)
	set(CMAKE_AUTOMOC ON)
	set(CMAKE_AUTORCC ON)
	add_subdirectory(xml_logs)
endif()
