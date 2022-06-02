# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.memory FRAGMENT align
	IMPORTS eagine.core.types
)

eagine_add_module(
	eagine.core.memory FRAGMENT address
	IMPORTS align eagine.core.types
)

eagine_add_module(
	eagine.core.memory FRAGMENT null_ptr
	IMPORTS eagine.core.types
)

eagine_add_module(
	eagine.core.memory
	INTERFACES
		memory
	FRAGMENTS
		align
		address
		null_ptr
)

