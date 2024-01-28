#  Copyright Matus Chochlik.
#  Distributed under the Boost Software License, Version 1.0.
#  See accompanying file LICENSE_1_0.txt or copy at
#  https://www.boost.org/LICENSE_1_0.txt
#
configure_file(EAGineEmbed.cmake.in EAGineEmbed.cmake @ONLY)
install(
	FILES
		"${CMAKE_CURRENT_BINARY_DIR}/EAGineEmbed.cmake"
		"EAGineEmbedCommon.cmake"
		DESTINATION "${EAGINE_CMAKE_CONFIG_DEST}")

configure_file(EAGineConfig.cmake.in EAGineConfig.cmake @ONLY)
install(
	FILES
		"${CMAKE_CURRENT_BINARY_DIR}/EAGineConfig.cmake"
		DESTINATION "${EAGINE_CMAKE_CONFIG_DEST}")

add_subdirectory(tools)
