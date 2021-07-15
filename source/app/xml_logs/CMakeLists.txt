# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt

# xml_logs
set(XML_LOGS_RESOURCES
	xml_logs.qrc
)
set(XML_LOGS_SOURCES
	xml_logs.cpp
	src/Theme.cpp
	src/EntryLog.cpp
	src/Backend.cpp
)
set(XML_LOGS_HEADERS
	internal_log_backend.hpp
	include/Theme.hpp
	include/EntryStorage.hpp
	include/EntryLog.hpp
	include/Backend.hpp
)
add_executable(
	eagine-xml_logs-gui
	${XML_LOGS_RESOURCES}
	${XML_LOGS_SOURCES}
	${XML_LOGS_HEADERS}
)

eagine_add_exe_analysis(eagine-xml_logs-gui)

target_include_directories(
	eagine-xml_logs-gui
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(
	eagine-xml_logs-gui
	PUBLIC
		EAGine::Core
		Qt5::Core
		Qt5::Widgets
		Qt5::Qml
		Qt5::Quick
)

