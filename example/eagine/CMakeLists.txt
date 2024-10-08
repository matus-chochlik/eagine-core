# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt
#
add_custom_target(eagine-core-examples ALL)
set_target_properties(
	eagine-core-examples
	PROPERTIES FOLDER "Example/Core")

function(eagine_install_example TARGET_NAME)
	install(
		TARGETS ${TARGET_NAME}
		COMPONENT core-examples
		DESTINATION share/eagine/example/core)
endfunction()

function(eagine_example_common EXAMPLE_NAME)
	add_executable(
		eagine-${EXAMPLE_NAME}
		EXCLUDE_FROM_ALL
		"${EXAMPLE_NAME}.cpp"
		${ARGN})

	add_dependencies(eagine-core-examples eagine-${EXAMPLE_NAME})
	eagine_add_exe_analysis(eagine-${EXAMPLE_NAME})
	eagine_target_modules(eagine-${EXAMPLE_NAME} std eagine.core)

	set_target_properties(
		eagine-${EXAMPLE_NAME}
		PROPERTIES FOLDER "Example/Core")

	eagine_install_example(eagine-${EXAMPLE_NAME})
endfunction()

eagine_example_common(valgrind)
eagine_example_common(integer_hash)
eagine_example_common(signal)
eagine_example_common(valid_if)
eagine_example_common(limit_cast)
eagine_example_common(str_format)
eagine_example_common(type_name)
eagine_example_common(args_within_limits)
eagine_example_common(optional_reference)
eagine_example_common(tribool)
eagine_example_common(trie)
eagine_example_common(environment)
eagine_example_common(reflect_enum)
eagine_example_common(reflect_struct)
eagine_example_common(system_info)
eagine_example_common(build_info)
eagine_example_common(user_info)
eagine_example_common(application_config)
eagine_example_common(encrypt_decrypt)
eagine_example_common(progress_bar)
eagine_example_common(progress)
eagine_example_common(from_string)
eagine_example_common(string_path)
eagine_example_common(edit_distance)
eagine_example_common(identifier)
eagine_example_common(interleaved_call)
eagine_example_common(overloaded)
eagine_example_common(hexdump)
eagine_example_common(bindump)
eagine_example_common(base64)
eagine_example_common(url)
eagine_example_common(scheduler)
eagine_example_common(slow_log)
eagine_example_common(log_histogram)
eagine_example_common(random_bytes)
eagine_example_common(compress_self)
eagine_example_common(scope_exit)
eagine_example_common(version)
eagine_example_common(uptime)
eagine_example_common(units)
eagine_example_common(animated_value)
eagine_example_common(variable_history)
eagine_example_common(network_sort)
eagine_example_common(make_index)
eagine_example_common(value_tree)
eagine_example_common(value_tree_overlay)
eagine_example_common(value_tree_visitor)
eagine_example_common(memoized)
#eagine_example_common(c_api_wrap)
eagine_example_common(dyn_lib_lookup)
eagine_example_common(serialize_basic)
eagine_example_common(serialize_string)
eagine_example_common(serialize_portable)
eagine_example_common(serialize_packed)
eagine_example_common(serialize_valtree)
eagine_example_common(serialize_fragmented)
eagine_example_common(sudoku_solver)
eagine_example_common(sudoku_tiling)
eagine_example_common(sudoku_noise)
eagine_example_common(simd_data)
eagine_example_common(simd_ident)
eagine_example_common(matrix)
#
eagine_example_common(embed_self)
eagine_embed_target_resources(TARGET eagine-embed_self ENABLE_SEARCH)
#
eagine_example_common(embed_packed)
eagine_embed_target_resources(
	TARGET eagine-embed_packed
	RESOURCES
		packedSelf "${CMAKE_CURRENT_SOURCE_DIR}/embed_packed.cpp"
	PACKED
	ENABLE_SEARCH)
#
eagine_example_common(value_tree_builder)
eagine_embed_target_resources(
	TARGET eagine-value_tree_builder
	RESOURCES
		json1 "value_tree_builder_1.json"
		json2 "value_tree_builder_2.json"
	ENABLE_SEARCH)
#
target_compile_options(
	eagine-valgrind
	PRIVATE $<$<BOOL:${EAGINE_CLANGXX_COMPILER}>:-Wno-unreachable-code>)

target_compile_options(
	eagine-limit_cast
	PRIVATE $<$<BOOL:${EAGINE_CLANGXX_COMPILER}>:-Wno-unreachable-code>)

eagine_add_license(core-examples)
eagine_add_debian_changelog(core-examples)

install(
	FILES
		eagine-application_config.yaml
	COMPONENT core-examples
	DESTINATION /etc/eagine)

eagine_add_package_property(
	core-examples
	GENERATOR DEBIAN
	VARIABLE PACKAGE_DEPENDS
	VALUE "\${CXX_RUNTIME_PKGS},\${EAGINE_CORE_RUNTIME_PKGS}")

eagine_add_package_property(
	core-examples
	GENERATOR DEBIAN
	VARIABLE DESCRIPTION
	VALUE "EAGine core examples.")

