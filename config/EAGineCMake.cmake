#  Copyright Matus Chochlik.
#  Distributed under the Boost Software License, Version 1.0.
#  See accompanying file LICENSE_1_0.txt or copy at
#  https://www.boost.org/LICENSE_1_0.txt
#
if(EAGINE_BUILD_MULTIARCH)
	set(EAGINE_CMAKE_CONFIG_DEST "lib/${EAGINE_BUILD_MULTIARCH}/cmake/EAGine")
else()
	set(EAGINE_CMAKE_CONFIG_DEST "lib/cmake/EAGine")
endif()
