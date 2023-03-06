# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.c_api PARTITION result
	IMPORTS
		eagine.core.concepts
		eagine.core.types
		eagine.core.utility
)

eagine_add_module(
	eagine.core.c_api PARTITION api_traits
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.valid_if
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
		eagine.core.concepts
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
	eagine.core.c_api PARTITION handle
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.container
)

eagine_add_module(
	eagine.core.c_api PARTITION object
	IMPORTS
		handle
		eagine.core.types
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
		handle buffer_data enum_class key_value_list
		eagine.core.types
		eagine.core.memory
		eagine.core.string
)

eagine_add_module(
	eagine.core.c_api PARTITION adapted_function
	IMPORTS
		handle result function parameter_map
		eagine.core.types
		eagine.core.memory;
)

eagine_add_module(
	eagine.core.c_api
	INTERFACES c_api
	PARTITIONS
		result
		api_traits
		key_value_list
		enum_class
		constant
		handle
		object
		function
		buffer_data
		parameter_map
		adapted_function
)

