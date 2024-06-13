#  Copyright Matus Chochlik.
#  Distributed under the Boost Software License, Version 1.0.
#  See accompanying file LICENSE_1_0.txt or copy at
#  https://www.boost.org/LICENSE_1_0.txt
#

# install the embed generator tool
install(
	PROGRAMS embed_generator
	COMPONENT embed-generator
	RENAME eagine-embed-generator
	PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE
	DESTINATION bin
)
