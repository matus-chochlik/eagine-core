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
		eagine.core.valid_if
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
		eagine.core.types
		eagine.core.memory
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.string PARTITION format
	IMPORTS
		c_str
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
		format
		base64
		base64dump
		bindump
		hexdump
		from_string
		keyboard_distance
	SOURCES
		bindump
		hexdump
		from_string
		keyboard_distance
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.math
		eagine.core.container
		eagine.core.utility
)

eagine_add_module_tests(
	eagine.core.string
	UNITS
		base64
		bindump
		environment
		from_string
		hexdump
		multi_byte
		list
		path
	IMPORTS
		eagine.core.types
		eagine.core.memory
	ENVIRONMENT
		EAGINE_TEST_VAR1=eagine
		EAGINE_TEST_VAR2=core
		EAGINE_TEST_VAR3=string
)

set_tests_properties(execute-test.eagine.core.string.base64 PROPERTIES COST 60)
set_tests_properties(execute-test.eagine.core.string.multi_byte PROPERTIES COST 45)
set_tests_properties(execute-test.eagine.core.string.list PROPERTIES COST 2)
