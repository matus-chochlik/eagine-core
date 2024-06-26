# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt
add_executable(eagine-core-sensors-log main.cpp)

eagine_add_exe_analysis(eagine-core-sensors-log)
eagine_target_modules(
	eagine-core-sensors-log
	std
	eagine.core)

eagine_embed_target_resources(
	TARGET eagine-core-sensors-log
	RESOURCES
		BoostLcnse "../../../LICENSE_1_0.txt"
		Manual     "resource/eagine-core-sensors-log.man"
		BashCmpltn "resource/eagine-core-sensors-log.bco"
	PACKED
	ENABLE_SEARCH)

install(
	TARGETS eagine-core-sensors-log
	COMPONENT core-logging
	RUNTIME DESTINATION bin)

install(
	DIRECTORY etc
	COMPONENT core-logging
	DESTINATION /)

eagine_add_bash_completion(core-logging "resource/eagine-core-sensors-log.bco")
eagine_add_manual(core-logging "resource/eagine-core-sensors-log.man")
eagine_add_package_conffiles(core-logging "etc")

