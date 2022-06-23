# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
#
add_custom_target(eagine-core-examples ALL)
set_target_properties(
	eagine-core-examples
	PROPERTIES FOLDER "Example/Core"
)

function(eagine_install_example TARGET_NAME)
	install(
		TARGETS ${TARGET_NAME}
		DESTINATION share/eagine/example/core
	)
endfunction()

function(eagine_example_common EXAMPLE_NAME)
	add_executable(
		eagine-${EXAMPLE_NAME}
		EXCLUDE_FROM_ALL
		"${EXAMPLE_NAME}.cpp"
		${ARGN}
	)
	eagine_add_exe_analysis(eagine-${EXAMPLE_NAME})
	add_dependencies(eagine-core-examples eagine-${EXAMPLE_NAME})
	target_link_libraries(
		eagine-${EXAMPLE_NAME}
		PUBLIC EAGine::Core
		PRIVATE ${ARGN}
	)

	set_target_properties(
		eagine-${EXAMPLE_NAME}
		PROPERTIES FOLDER "Example/Core"
	)
	eagine_install_example(eagine-${EXAMPLE_NAME})
endfunction()

function(eagine_example_module EXAMPLE_NAME)
	add_executable(
		eagine-${EXAMPLE_NAME}
		EXCLUDE_FROM_ALL
		"${EXAMPLE_NAME}.cpp"
		${ARGN}
	)
	add_dependencies(eagine-core-examples eagine-${EXAMPLE_NAME})
	if(${WITH_MODULES})
		eagine_target_modules(eagine-${EXAMPLE_NAME} eagine.core)
	else()
		eagine_add_exe_analysis(eagine-${EXAMPLE_NAME})
		target_link_libraries(
			eagine-${EXAMPLE_NAME}
			PUBLIC EAGine::Core
			PRIVATE ${ARGN}
		)
	endif()

	set_target_properties(
		eagine-${EXAMPLE_NAME}
		PROPERTIES FOLDER "Example/Core"
	)
	eagine_install_example(eagine-${EXAMPLE_NAME})
endfunction()

eagine_example_common(assert)
eagine_example_common(valgrind)
eagine_example_module(array_size)
eagine_example_common(integer_hash)
eagine_example_module(signal_switch)
eagine_example_module(valid_if)
eagine_example_module(limit_cast)
eagine_example_module(str_format)
eagine_example_module(type_name)
eagine_example_module(args_within_limits)
eagine_example_module(extract)
eagine_example_common(tribool)
eagine_example_module(environment)
eagine_example_common(reflect_enum)
eagine_example_common(reflect_struct)
eagine_example_common(system_info)
eagine_example_common(build_info)
eagine_example_common(user_info)
eagine_example_common(application_config)
eagine_example_module(progress_bar)
eagine_example_common(progress)
eagine_example_common(from_string)
eagine_example_module(string_path)
eagine_example_common(edit_distance)
eagine_example_module(identifier)
eagine_example_common(id_tag)
eagine_example_module(interleaved_call)
eagine_example_module(overloaded)
eagine_example_common(hexdump)
eagine_example_common(bindump)
eagine_example_module(base64)
eagine_example_common(url)
eagine_example_common(slow_log)
eagine_example_common(log_histogram)
eagine_example_common(random_bytes)
eagine_example_common(compress_self)
eagine_example_module(scope_exit)
eagine_example_common(version)
eagine_example_common(uptime)
eagine_example_common(units)
eagine_example_module(animated_value)
eagine_example_common(variable_history)
eagine_example_module(network_sort)
eagine_example_module(make_index)
eagine_example_common(value_tree)
eagine_example_common(value_tree_overlay)
eagine_example_common(memoized)
eagine_example_common(c_api_wrap)
eagine_example_common(dyn_lib_lookup)
eagine_example_common(serialize_basic)
eagine_example_common(serialize_string)
eagine_example_common(serialize_portable)
eagine_example_common(serialize_packed)
eagine_example_common(serialize_fragmented)
eagine_example_common(sudoku_solver)
eagine_example_common(sudoku_tiling)
eagine_example_common(sudoku_noise)
#
eagine_example_common(embed_self)
eagine_embed_target_resources(TARGET eagine-embed_self)

install(
	FILES
		eagine-application_config.yaml
	DESTINATION etc/eagine
)
