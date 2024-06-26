# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt
eagine_common_import_lib(
	PREFIX LIBPQ
	PKGCONFIG libpq
	HEADER "libpq-fe.h"
	LIBRARY pq)

eagine_common_import_lib(
	PREFIX LIBCURL
	PKGCONFIG libcurl
	HEADER "curl/curl.h"
	LIBRARY curl)

eagine_add_module(
	eagine.core.log_server
	COMPONENT core-dev
	PARTITION interfaces
	IMPORTS std eagine.core)

eagine_add_module(
	eagine.core.log_server
	COMPONENT core-dev
	PARTITION utilities
	IMPORTS std eagine.core interfaces)

eagine_add_module(
	eagine.core.log_server
	COMPONENT core-dev
	SOURCES
		implementation
		utilities
		internal_backend
		istream_reader
		asio_reader
		json_parser
		text_tree_sink
		influxdb_sink
		libpq_sink
		ostream_output
		asio_output
		combined_sink
		combined_output
	IMPORTS std eagine.core
	PRIVATE_INCLUDE_DIRECTORIES
		"${PROJECT_SOURCE_DIR}/submodules/asio/asio/include"
	PRIVATE_LINK_LIBRARIES
		EAGine::Deps::LIBCURL
		EAGine::Deps::LIBPQ)

add_executable(eagine-core-log-server main.cpp)

eagine_add_exe_analysis(eagine-core-log-server)
eagine_target_modules(
	eagine-core-log-server
	std
	eagine.core
	eagine.core.log_server)

eagine_embed_target_resources(
	TARGET eagine-core-log-server
	RESOURCES
		BoostLcnse "../../../LICENSE_1_0.txt"
		Manual     "resource/eagine-core-log-server.man"
		BashCmpltn "resource/eagine-core-log-server.bco"
		PSQLSchema "postgresql/eagilog.sql"
	PACKED
	ENABLE_SEARCH)

install(
	TARGETS eagine-core-log-server
	COMPONENT core-logging
	RUNTIME DESTINATION bin)

eagine_add_license(core-logging)
eagine_add_debian_changelog(core-logging)
eagine_add_bash_completion(core-logging "resource/eagine-core-log-server.bco")
eagine_add_manual(core-logging "resource/eagine-core-log-server.man")

eagine_add_package_property(
	core-logging
	GENERATOR DEBIAN
	VARIABLE PACKAGE_DEPENDS
	VALUE "\${CXX_RUNTIME_PKGS},\${EAGINE_CORE_RUNTIME_PKGS}")

eagine_add_package_property(
	core-logging
	GENERATOR DEBIAN
	VARIABLE DESCRIPTION
	VALUE "EAGine logging-related applications.")

install(
	FILES "postgresql/eagilog.sql"
	COMPONENT core-docker
	PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
	DESTINATION share/eagine)

