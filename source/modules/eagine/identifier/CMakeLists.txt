# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.identifier PARTITION decl
	IMPORTS
		eagine.core.types
		eagine.core.memory
)

eagine_add_module(
	eagine.core.identifier PARTITION construct
	IMPORTS
		decl
		eagine.core.types
)

eagine_add_module(
	eagine.core.identifier PARTITION message_id
	IMPORTS
		decl
		eagine.core.types
		eagine.core.vectorization
)

eagine_add_module(
	eagine.core.identifier
	INTERFACES
		identifier
	PARTITIONS
		decl
		construct
		message_id
	SOURCES
		construct_impl
)
