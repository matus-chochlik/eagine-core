# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt

install(
	PROGRAMS git-filter.py
	COMPONENT core-tools
	RENAME eagine-git-filter
	DESTINATION bin)
install(
	PROGRAMS ensure-main-key.sh
	COMPONENT core-tools
	RENAME eagine-ensure-main-key
	DESTINATION bin)
install(
	PROGRAMS fmd-modify.py
	COMPONENT core-tools
	RENAME eagine-fmd-modify
	DESTINATION bin)
install(
	PROGRAMS fmd-get.py
	COMPONENT core-tools
	RENAME eagine-fmd-get
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
install(
	PROGRAMS log-plot.py
	COMPONENT core-tools
	RENAME eagilog-plot
	DESTINATION bin)

install(
	PROGRAMS procman.py
	COMPONENT core-tools
	RENAME eagine-procman
	DESTINATION bin)

add_subdirectory(log_server)
add_subdirectory(sensors_log)

find_package(Qt5 COMPONENTS Core Widgets Xml Qml Quick)
if(Qt5_FOUND)
	set(CMAKE_AUTOMOC ON)
	set(CMAKE_AUTORCC ON)
	add_subdirectory(xml_logs)
endif()
