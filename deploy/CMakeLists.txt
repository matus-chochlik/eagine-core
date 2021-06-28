# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
#
find_program(BASH_COMMAND bash)
find_program(DPKG_DEB_COMMAND dpkg-deb)
if(BASH_COMMAND AND DPKG_DEB_COMMAND)
	add_subdirectory(debian)
endif()
