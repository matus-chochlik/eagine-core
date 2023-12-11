# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
add_executable(eagine-core-sensors-log main.cpp)

eagine_add_exe_analysis(eagine-core-sensors-log)
eagine_target_modules(
	eagine-core-sensors-log
	std
	eagine.core)

