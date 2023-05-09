# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt

add_subdirectory(modules)
add_subdirectory(app)

set(EAGINE_CORE_EXPORT_TARGETS)

if(TARGET eagine-xml_logs-gui)
	# TODO: re-enable this when clang is fixed
	# list(APPEND EAGINE_CORE_EXPORT_TARGETS eagine-xml_logs-gui)
endif()

install(
	TARGETS ${EAGINE_CORE_EXPORT_TARGETS}
	EXPORT Core
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
	PUBLIC_HEADER DESTINATION include
)


