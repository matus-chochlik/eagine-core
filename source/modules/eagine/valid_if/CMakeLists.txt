# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.valid_if
	FRAGMENT decl
	IMPORTS
		eagine.core.types
		eagine.core.concepts
)

eagine_add_module(
	eagine.core.valid_if FRAGMENT always
	IMPORTS decl eagine.core.concepts
)
eagine_add_module(
	eagine.core.valid_if FRAGMENT never
	IMPORTS decl eagine.core.concepts
)
eagine_add_module(
	eagine.core.valid_if FRAGMENT not_empty
	IMPORTS decl eagine.core.concepts
)
eagine_add_module(
	eagine.core.valid_if FRAGMENT compare
	IMPORTS decl eagine.core.concepts
)

eagine_add_module(
	eagine.core.valid_if
	INTERFACES
		valid_if
	FRAGMENTS
		decl
		always
		never
		compare
		not_empty
)

