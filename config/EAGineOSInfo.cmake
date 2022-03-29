#  Copyright Matus Chochlik.
#  Distributed under the Boost Software License, Version 1.0.
#  See accompanying file LICENSE_1_0.txt or copy at
#   http://www.boost.org/LICENSE_1_0.txt
#
find_program(LSB_RELEASE_COMMAND lsb_release)

if(LSB_RELEASE_COMMAND)
	execute_process(
		COMMAND ${LSB_RELEASE_COMMAND} "-d" "-s"
		WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		OUTPUT_VARIABLE EAGINE_OS_NAME
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	execute_process(
		COMMAND ${LSB_RELEASE_COMMAND} "-c" "-s"
		WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		OUTPUT_VARIABLE EAGINE_OS_CODE_NAME
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
endif()
