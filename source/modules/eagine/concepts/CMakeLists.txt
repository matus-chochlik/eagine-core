# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.concepts FRAGMENT utils
)

eagine_add_module(
	eagine.core.concepts FRAGMENT type_traits
	IMPORTS utils
)

eagine_add_module(
	eagine.core.concepts
	INTERFACES concepts
	FRAGMENTS
		utils
		type_traits
		does_not_hide
		extractable
)

