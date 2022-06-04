# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.identifier FRAGMENT decl
	IMPORTS
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.identifier FRAGMENT message_id
	IMPORTS decl eagine.core.types
)

eagine_add_module(
	eagine.core.identifier
	INTERFACES
		identifier
	FRAGMENTS
		decl
		message_id
)

