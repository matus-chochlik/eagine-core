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
	eagine.core.runtime PARTITION program_args
	IMPORTS
		eagine.core.concepts
		eagine.core.types
		eagine.core.memory
		eagine.core.string
		eagine.core.valid_if
)

eagine_add_module(
	eagine.core.runtime PARTITION compression
	IMPORTS
		eagine.core.types
		eagine.core.memory
		eagine.core.utility
)

eagine_add_module(
	eagine.core.runtime PARTITION main_ctx
	IMPORTS
		program_args
		eagine.core.types
)

eagine_add_module(
	eagine.core.runtime
	INTERFACES
		runtime
	PARTITIONS
		signal_switch
		process
		program_args
		compression
		main_ctx
	SOURCES
		compression_impl
)
