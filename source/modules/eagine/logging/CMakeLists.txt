# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.logging FRAGMENT severity
	IMPORTS eagine.core.reflection
)

eagine_add_module(
	eagine.core.logging FRAGMENT entry_arg
	IMPORTS
		eagine.core.types
		eagine.core.reflection
		eagine.core.identifier
)

eagine_add_module(
	eagine.core.logging FRAGMENT backend
	IMPORTS
		entry_arg
		severity
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
)

eagine_add_module(
	eagine.core.logging FRAGMENT null_backend
	IMPORTS
		backend
		eagine.core.types
		eagine.core.memory
		eagine.core.identifier
)

eagine_add_module(
	eagine.core.logging
	INTERFACES
		logging
	FRAGMENTS
		entry_arg
		severity
		backend
		null_backend
)


