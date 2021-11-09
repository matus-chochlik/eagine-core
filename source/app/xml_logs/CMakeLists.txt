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
	src/Connection.cpp
	src/Server.cpp
	src/Theme.cpp
	src/EntryStorage.cpp
	src/EntryFormat.cpp
	src/EntryLog.cpp
	src/EntryViewModel.cpp
	src/EntryFilterModel.cpp
	src/EntryListModel.cpp
	src/ActivityStorage.cpp
	src/ActivityListModel.cpp
	src/StreamListModel.cpp
	src/ProgressViewModel.cpp
	src/ChartsViewModel.cpp
	src/StreamViewModel.cpp
	src/Backend.cpp
)
set(XML_LOGS_HEADERS
	internal_log_backend.hpp
	include/Utility.hpp
	include/Connection.hpp
	include/Server.hpp
	include/Theme.hpp
	include/EntryStorage.hpp
	include/EntryFormat.hpp
	include/EntryLog.hpp
	include/EntryViewModel.hpp
	include/EntryFilterModel.hpp
	include/EntryListModel.hpp
	include/ActivityStorage.hpp
	include/ActivityListModel.hpp
	include/StreamListModel.hpp
	include/ProgressViewModel.hpp
	include/ChartsViewModel.hpp
	include/StreamViewModel.hpp
	include/Backend.hpp
)
add_executable(
	eagine-xml_logs-gui
	${XML_LOGS_RESOURCES}
	${XML_LOGS_SOURCES}
	${XML_LOGS_HEADERS}
)

eagine_add_exe_analysis(eagine-xml_logs-gui)

target_compile_options(
	eagine-xml_logs-gui
	PRIVATE $<$<BOOL:${EAGINE_GNUCXX_COMPILER}>:-Wno-extra-semi-stmt;-Wno-redundant-parens>
)
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
		Qt5::Xml
		Qt5::Qml
		Qt5::Quick
)

