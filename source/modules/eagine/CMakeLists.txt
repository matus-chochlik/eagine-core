# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt

add_subdirectory(build_config)
add_subdirectory(types)
add_subdirectory(debug)
add_subdirectory(memory)
add_subdirectory(reflection)
add_subdirectory(simd)
add_subdirectory(identifier)
add_subdirectory(valid_if)
add_subdirectory(container)
add_subdirectory(utility)
add_subdirectory(build_info)
add_subdirectory(units)
add_subdirectory(math)
add_subdirectory(string)
add_subdirectory(c_api)
add_subdirectory(runtime)
add_subdirectory(logging)
add_subdirectory(console)
add_subdirectory(progress)
add_subdirectory(value_tree)
add_subdirectory(serialization)
add_subdirectory(main_ctx)
add_subdirectory(resource)
add_subdirectory(app)

eagine_add_module(
	eagine.core
	COMPONENT core-dev
	SUBMODULES
		build_config
		types
		debug
		memory
		identifier
		container
		utility
		valid_if
		simd
		reflection
		build_info
		units
		math
		string
		c_api
		runtime
		logging
		console
		progress
		value_tree
		serialization
		main_ctx
		resource
		app
	INTERFACE_LINK_LIBRARIES
		ryml
		systemd
		z)

eagine_add_module_tests(
	eagine.core
	UNITS
		embed
		valtree
	IMPORTS
		std
		eagine.core)

eagine_embed_target_resources(
	TARGET test.eagine.core.embed
	RESOURCES
		CMakeLists "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt"
	ENABLE_SEARCH)

eagine_add_license(core-dev)
eagine_add_debian_changelog(core-dev)

eagine_add_package_property(
	core-dev
	GENERATOR DEBIAN
	VARIABLE PACKAGE_DEPENDS
	VALUE "cmake,libsystemd-dev,zlib1g-dev")

eagine_add_package_property(
	core-dev
	GENERATOR DEBIAN
	VARIABLE PACKAGE_SUGGESTS
	VALUE "ninja-build")

eagine_add_package_property(
	core-dev
	GENERATOR DEBIAN
	VARIABLE DESCRIPTION
	VALUE "Collection of various modern C++ utilities.")

