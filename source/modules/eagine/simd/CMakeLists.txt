# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt
eagine_add_module(
	eagine.core.simd
	COMPONENT core-dev
	PARTITION ops
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory)

eagine_add_module(
	eagine.core.simd
	COMPONENT core-dev)

eagine_add_module_tests(
	eagine.core.simd
	UNITS
		abs
		axis
		compare
		data
		diff
		from
		hsum
		shuffle
		view
	IMPORTS
		std
		eagine.core.types
		eagine.core.memory)
