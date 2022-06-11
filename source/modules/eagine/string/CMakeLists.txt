# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.string PARTITION multi_byte
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.valid_if
		eagine.core.utility
)

eagine_add_module(
	eagine.core.string PARTITION list
	IMPORTS
		multi_byte
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.string PARTITION path
	IMPORTS
		list
		eagine.core.types
		eagine.core.memory
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.string PARTITION c_str
	IMPORTS
		eagine.core.concepts
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.string PARTITION algorithm
	IMPORTS
		eagine.core.memory
)

eagine_add_module(
	eagine.core.string PARTITION environment
	IMPORTS
		c_str
		eagine.core.memory
)

eagine_add_module(
	eagine.core.string PARTITION substitution
	IMPORTS
		c_str
		eagine.core.types
		eagine.core.memory
		eagine.core.valid_if
		eagine.core.utility
)

eagine_add_module(
	eagine.core.string PARTITION format
	IMPORTS
		substitution
		eagine.core.types
		eagine.core.memory
		eagine.core.utility
)

eagine_add_module(
	eagine.core.string PARTITION base64
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.utility
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.string PARTITION base64dump
	IMPORTS
		base64
		eagine.core.types
		eagine.core.memory
		eagine.core.utility
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.string PARTITION bindump
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.utility
)

eagine_add_module(
	eagine.core.string PARTITION hexdump
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.utility
)

eagine_add_module(
	eagine.core.string PARTITION from_string
	IMPORTS
		c_str
		eagine.core.types
		eagine.core.math
		eagine.core.memory
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.string PARTITION keyboard_distance
	IMPORTS
		eagine.core.math
		eagine.core.memory
		eagine.core.container
)

eagine_add_module(
	eagine.core.string
	INTERFACES
		string
	PARTITIONS
		multi_byte
		list
		path
		c_str
		algorithm
		environment
		substitution
		format
		base64
		base64dump
		bindump
		hexdump
		from_string
		keyboard_distance
	SOURCES
		bindump_impl
		hexdump_impl
		from_string_impl
		keyboard_distance_impl
)


