# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt

set(EAGINE_LIB_SOURCES
	debug.cpp
	main_wrap.cpp
	git_info.cpp
	build_info.cpp
	compiler_info.cpp
	system_info.cpp
	user_info.cpp
	identifier.cpp
	from_string.cpp
	edit_distance.cpp
	logging.cpp
	file_contents.cpp
	input_data.cpp
	compression.cpp
	str_var_subst.cpp
	random.cpp
	url.cpp
	value_tree_json.cpp
	value_tree_yaml.cpp
	value_tree_filesystem.cpp
	value_tree_wrappers.cpp
	watchdog.cpp
)

add_library(
	eagine-core
	${EAGINE_LIB_SOURCES}
)
eagine_add_lib_analysis(eagine-core)

target_compile_definitions(
	eagine-core PUBLIC EAGINE_CORE_LIBRARY=1
)

target_include_directories(
	eagine-core
	PUBLIC
		"$<INSTALL_INTERFACE:include>"
		"$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>"
		"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
	PRIVATE
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
		"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/submodules/asio/asio/include>"
		"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/submodules/rapidjson/include>"
)

target_link_libraries(eagine-core PRIVATE ryml)

target_link_libraries(
	eagine-core
	PUBLIC
		EAGine::Required
		EAGopt::Stacktrace
		Threads::Threads
	PRIVATE
		EAGopt::ZLIB
		EAGopt::BoostSpirit
		SYSTEMD::SYSTEMD
)

set_target_properties(
	eagine-core PROPERTIES
	VERSION ${EAGINE_VERSION}
	FOLDER "Library/EAGine"
)

