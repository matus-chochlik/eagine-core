# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt

define_property(
	TARGET
	PROPERTY EAGINE_MODULE_PROPER
	BRIEF_DOCS "Main module name"
	FULL_DOCS "Name of a module or the name of the main module of a partition"
)
define_property(
	TARGET
	PROPERTY EAGINE_MODULE_PARTITION
	BRIEF_DOCS "Module partition name"
	FULL_DOCS "Name of a module partition"
)
define_property(
	TARGET
	PROPERTY EAGINE_MODULE_ID
	BRIEF_DOCS "Full module name"
	FULL_DOCS "Full name of a module or a module partition"
)

define_property(
	TARGET
	PROPERTY EAGINE_MODULE_INTERFACES
	BRIEF_DOCS "Module interfaces"
	FULL_DOCS "List of C++ module interface source names"
)
define_property(
	TARGET
	PROPERTY EAGINE_MODULE_PARTITIONS
	BRIEF_DOCS "Module partitions"
	FULL_DOCS "List of C++ module partition source names"
)
define_property(
	TARGET
	PROPERTY EAGINE_MODULE_IMPORTS
	BRIEF_DOCS "Module imports"
	FULL_DOCS "List of C++ module import names"
)

if(${EAGINE_CLANGXX_COMPILER})
	include(EAGineCXXModulesClang)
elseif(${EAGINE_GXX_COMPILER})
	include(EAGineCXXModulesGNU)
else()
	message(FATAL_ERROR "Modules are not supported with this compiler")
endif()
