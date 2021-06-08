#  Copyright Matus Chochlik.
#  Distributed under the Boost Software License, Version 1.0.
#  See accompanying file LICENSE_1_0.txt or copy at
#   http://www.boost.org/LICENSE_1_0.txt
#
find_program(GIT_COMMAND git)

if(GIT_COMMAND)
	set(EAGINE_HAS_GIT_INFO 1)
	execute_process(
		COMMAND ${GIT_COMMAND} "rev-parse" "HEAD"
		WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		OUTPUT_VARIABLE EAGINE_GIT_HASH_ID
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	execute_process(
		COMMAND ${GIT_COMMAND} "rev-parse" "--abbrev-ref" "HEAD"
		WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		OUTPUT_VARIABLE EAGINE_GIT_BRANCH
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	execute_process(
		COMMAND ${GIT_COMMAND} "show" "-s" "--format=%aD" "HEAD"
		WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		OUTPUT_VARIABLE EAGINE_GIT_DATE
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	execute_process(
		COMMAND ${GIT_COMMAND} "describe" "--always" "HEAD"
		WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		OUTPUT_VARIABLE EAGINE_GIT_DESCRIBE
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
if("${EAGINE_VERSION}" STREQUAL "${EAGINE_GIT_DESCRIBE}")
		set(EAGINE_GIT_COMMITS_SINCE_VERSION 0)
	else()
		execute_process(
			COMMAND ${GIT_COMMAND} "rev-list" "--count" "${EAGINE_VERSION}^2..HEAD"
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
			OUTPUT_VARIABLE EAGINE_GIT_COMMITS_SINCE_VERSION
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)
		if(NOT EAGINE_GIT_COMMITS_SINCE_VERSION)
			set(EAGINE_GIT_COMMITS_SINCE_VERSION 0)
		endif()
		math(
			EXPR EAGINE_GIT_COMMITS_SINCE_VERSION
			"${EAGINE_GIT_COMMITS_SINCE_VERSION}-1"
		)
	endif()
	string(
		REGEX REPLACE
		"^([0-9](.[1-9]+)+)(.0)+$"
		"\\1"
		EAGINE_GIT_VERSION
		"${EAGINE_VERSION}.${EAGINE_GIT_COMMITS_SINCE_VERSION}"
	)
	string(
		REGEX REPLACE
		"^([0-9]).([0-9]+).([0-9]+)$"
		"\\1"
		EAGINE_GIT_VERSION_MAJOR
		"${EAGINE_VERSION}"
	)
	string(
		REGEX REPLACE
		"^([0-9]).([0-9]+).([0-9]+)$"
		"\\2"
		EAGINE_GIT_VERSION_MINOR
		"${EAGINE_VERSION}"
	)
	string(
		REGEX REPLACE
		"^([0-9]).([0-9]+).([0-9]+)$"
		"\\3"
		EAGINE_GIT_VERSION_PATCH
		"${EAGINE_VERSION}"
	)
else()
	set(EAGINE_HAS_GIT_INFO 0)
endif()

configure_file(
	${PROJECT_SOURCE_DIR}/config/eagine/config/git_info.hpp.in
	${PROJECT_BINARY_DIR}/include/eagine/config/git_info.hpp
	@ONLY
)

