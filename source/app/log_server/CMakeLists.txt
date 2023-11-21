# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt

add_executable(
	eagine-core-log-server
		utilities.cpp
		istream_reader.cpp
		json_parser.cpp
		ostream_sink.cpp
		implementation.cpp
		internal_backend.cpp
		main.cpp
)
eagine_add_exe_analysis(eagine-core-log-server)
eagine_target_modules(eagine-core-log-server std eagine.core)

