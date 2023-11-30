# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_common_import_lib(
	PREFIX LIBPQ
	 PKGCONFIG libpq
	 HEADER "libpq-fe.h"
	 LIBRARY pq)

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
		libpq_sink
		ostream_output
		asio_output
		combined_output
	IMPORTS std eagine.core
	PRIVATE_INCLUDE_DIRECTORIES
		"${PROJECT_SOURCE_DIR}/submodules/asio/asio/include"
	PRIVATE_LINK_LIBRARIES
		EAGine::Deps::LIBPQ)

add_executable(eagine-core-log-server main.cpp)

eagine_add_exe_analysis(eagine-core-log-server)
eagine_target_modules(
	eagine-core-log-server
	std
	eagine.core
	eagine.core.log_server)

