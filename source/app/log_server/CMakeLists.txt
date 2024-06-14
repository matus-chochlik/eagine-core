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
	eagine.core.log_server PARTITION interfaces
	IMPORTS std eagine.core)

eagine_add_module(
	eagine.core.log_server PARTITION utilities
	IMPORTS std eagine.core interfaces)

eagine_add_module(
	eagine.core.log_server
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

add_custom_command(
	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/eagine-core-log-server.1.gz"
	COMMAND ${GZIP_COMMAND} -cn9
				"${CMAKE_CURRENT_SOURCE_DIR}/resource/eagine-core-log-server.man"
				> "${CMAKE_CURRENT_BINARY_DIR}/eagine-core-log-server.1.gz"
	DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/resource/eagine-core-log-server.man")

add_custom_target("eagine-core-log-server-man"
	ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/eagine-core-log-server.1.gz")

install(
	FILES "${CMAKE_CURRENT_BINARY_DIR}/eagine-core-log-server.1.gz"
	COMPONENT core-logging
	PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
	DESTINATION /usr/share/man/man1)

install(
	FILES "resource/eagine-core-log-server.bco"
	COMPONENT core-logging
	RENAME eagine-core-log-server
	PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
	DESTINATION /usr/share/bash-completion/completions)

eagine_add_license(core-logging)
eagine_add_debian_changelog(core-logging)
