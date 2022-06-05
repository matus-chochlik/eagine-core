# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.string FRAGMENT multi_byte
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.valid_if
		eagine.core.utility
)

eagine_add_module(
	eagine.core.string FRAGMENT list
	IMPORTS
		multi_byte
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.string FRAGMENT path
	IMPORTS
		list
		eagine.core.types
		eagine.core.memory
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.string FRAGMENT c_str
	IMPORTS
		eagine.core.concepts
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.string FRAGMENT substitution
	IMPORTS
		c_str
		eagine.core.types
		eagine.core.memory
		eagine.core.valid_if
		eagine.core.utility
)

eagine_add_module(
	eagine.core.string FRAGMENT format
	IMPORTS
		substitution
		eagine.core.types
		eagine.core.memory
		eagine.core.utility
)

eagine_add_module(
	eagine.core.string FRAGMENT hexdump
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.utility
)

eagine_add_module(
	eagine.core.string
	INTERFACES
		string
	FRAGMENTS
		multi_byte
		list
		path
		c_str
		substitution
		format
		hexdump
	SOURCES
		hexdump_impl
)


