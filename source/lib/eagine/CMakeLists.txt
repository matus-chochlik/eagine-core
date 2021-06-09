# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt

add_library(
	eagine-core
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
eagine_add_lib_analysis(eagine-core)

target_compile_definitions(
	eagine-core PUBLIC EAGINE_CORE_LIBRARY=1
)

target_include_directories(
	eagine-core
	PRIVATE
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
)

target_link_libraries(
	eagine-core
	PUBLIC
		eagine-core-generated-headers
		eagine-core-headers
		EAGine::Core::Deps::System
		EAGine::Core::Deps::ZLIB
		EAGine::Core::Deps::Stacktrace
		EAGine::Core::Deps::BoostSpirit
		Threads::Threads
		SYSTEMD::SYSTEMD
	PRIVATE
		eagine-core-private-headers
		ryml
)

set_target_properties(
	eagine-core PROPERTIES
	VERSION ${EAGINE_VERSION}
	FOLDER "Library/EAGine"
)

