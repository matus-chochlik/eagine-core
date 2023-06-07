# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.c_api PARTITION result
	IMPORTS
		eagine.core.types
		eagine.core.utility
)

eagine_add_module(
	eagine.core.c_api PARTITION api_traits
	IMPORTS
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.c_api PARTITION key_value_list
	IMPORTS
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.c_api PARTITION enum_class
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.container
		eagine.core.identifier
)

eagine_add_module(
	eagine.core.c_api PARTITION constant
	IMPORTS
		enum_class
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.c_api PARTITION object
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.container
)

eagine_add_module(
	eagine.core.c_api PARTITION function
	IMPORTS
		api_traits
		result
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.c_api PARTITION buffer_data
	IMPORTS
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.c_api PARTITION parameter_map
	IMPORTS
		object buffer_data enum_class key_value_list
		eagine.core.types
		eagine.core.memory
		eagine.core.string
)

eagine_add_module(
	eagine.core.c_api PARTITION adapted_function
	IMPORTS
		object result function parameter_map
		eagine.core.types
		eagine.core.memory;
		eagine.core.utility;
)

eagine_add_module(
	eagine.core.c_api
)

eagine_add_module_tests(
	eagine.core.c_api
	UNITS
		buffer_data
		key_value_list
		result
	IMPORTS
		eagine.core.debug
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.utility
		eagine.core.identifier
		eagine.core.container
		eagine.core.valid_if
		eagine.core.units
)
