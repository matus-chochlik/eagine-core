# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.runtime PARTITION process
	IMPORTS
		eagine.core.types
		eagine.core.utility
)

eagine_add_module(
	eagine.core.runtime PARTITION basic_config
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.utility
)

eagine_add_module(
	eagine.core.runtime PARTITION program_args
	IMPORTS
		eagine.core.debug
		eagine.core.concepts
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.runtime PARTITION url
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.valid_if
		eagine.core.container
)

eagine_add_module(
	eagine.core.runtime PARTITION compression
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.utility
)

eagine_add_module(
	eagine.core.runtime PARTITION input_data
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.utility
)

eagine_add_module(
	eagine.core.runtime PARTITION executable_module
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.string
)

eagine_add_module(
	eagine.core.runtime PARTITION file_contents
	IMPORTS
		input_data
		eagine.core.types
		eagine.core.memory
		eagine.core.utility
)

eagine_add_module(
	eagine.core.runtime PARTITION value_with_history
	IMPORTS
		eagine.core.types
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.runtime PARTITION workshop
	IMPORTS
		eagine.core.types
)

eagine_add_module(
	eagine.core.runtime
	INTERFACES
		runtime
	PARTITIONS
		signal_switch
		basic_config
		process
		program_args
		url
		input_data
		executable_module
		file_contents
		compression
		value_with_history
		workshop
	SOURCES
		url_impl
		input_data_impl
		executable_module_impl
		file_contents_impl
		compression_impl
)
target_link_libraries(
	eagine.core.runtime
	PUBLIC
		EAGine::Deps::System
		EAGine::Deps::SYSTEMD
		EAGine::Deps::ZLIB
)
