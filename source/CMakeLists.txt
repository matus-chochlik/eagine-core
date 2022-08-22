# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt

if(${WITH_MODULES})
	add_subdirectory(modules)
else()
	add_subdirectory(lib)
endif()
add_subdirectory(app)

if(TARGET eagine-core)
	add_library(EAGine::Core INTERFACE IMPORTED GLOBAL)
	target_link_libraries(EAGine::Core INTERFACE eagine-core)
	set(
		EAGINE_CORE_EXPORT_TARGETS
			eagine-core-generated-headers
			eagine-core-private-headers
			eagine-core-headers
			eagine-core
	)

	if(TARGET eagine-xml_logs-gui)
		list(APPEND EAGINE_CORE_EXPORT_TARGETS eagine-xml_logs-gui)
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

	install(
		EXPORT Core
		NAMESPACE EAGine::
		FILE Core.cmake
		DESTINATION lib/cmake/EAGine
	)
endif()
