# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.c_api PARTITION result
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
		eagine.core.utility
)

eagine_add_module(
	eagine.core.c_api PARTITION api_traits
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.c_api PARTITION key_value_list
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.c_api PARTITION enum_class
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
		eagine.core.container
		eagine.core.identifier
)

eagine_add_module(
	eagine.core.c_api PARTITION constant
	IMPORTS
		std enum_class
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.c_api PARTITION object
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
		eagine.core.container
)

eagine_add_module(
	eagine.core.c_api PARTITION function
	IMPORTS
		std api_traits result
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.c_api PARTITION buffer_data
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.c_api PARTITION parameter_map
	IMPORTS
		std object buffer_data
		enum_class key_value_list
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.math
)

eagine_add_module(
	eagine.core.c_api PARTITION adapted_function
	IMPORTS
		std object result
		function parameter_map
		eagine.core.types
		eagine.core.memory;
		eagine.core.utility;
)

eagine_add_module(
	eagine.core.c_api PARTITION reference
	IMPORTS
		std
		eagine.core.types
)

eagine_add_module(
	eagine.core.c_api
)

eagine_add_module_tests(
	eagine.core.c_api
	UNITS
		buffer_data
		enum_class
		enum_bitfield
		key_value_list
		result
	IMPORTS
		std
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
