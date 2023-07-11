# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt

install(
	PROGRAMS ensure_main_key.sh
	RENAME eagine-ensure-main-key
	DESTINATION bin
)
install(
	PROGRAMS fmd_modify.py
	RENAME eagine-fmd-modify
	DESTINATION bin
)
install(
	PROGRAMS xml_logs-cl.py
	RENAME eagine-xml_logs-cl
	DESTINATION bin
)
install(
	PROGRAMS xml_logs-psql.py
	RENAME eagine-xml_logs-psql
	DESTINATION bin
)
install(
	PROGRAMS log-schema-stats.sh
	RENAME eagine-log-schema-stats
	DESTINATION bin
)

install(
	PROGRAMS servicectl.py
	RENAME eagine-servicectl
	DESTINATION bin
)

install(
	PROGRAMS procman.py
	RENAME eagine-procman
	DESTINATION bin
)

find_package(Qt5 COMPONENTS Core Widgets Xml Qml Quick)
if(Qt5_FOUND)
	set(CMAKE_AUTOMOC ON)
	set(CMAKE_AUTORCC ON)
	add_subdirectory(xml_logs)
endif()
