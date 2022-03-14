# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
#
set(HEADERS
    eagine/animated_value.hpp
    eagine/anything.hpp
    eagine/app_config.hpp
    eagine/args_within_limits.hpp
    eagine/array_size.hpp
    eagine/assert.hpp
    eagine/base64dump.hpp
    eagine/base64.hpp
    eagine/bindump.hpp
    eagine/bit_density.hpp
    eagine/biteset.hpp
    eagine/bitfield.hpp
    eagine/bool_aggregate.hpp
    eagine/branch_predict.hpp
    eagine/buffer_data.hpp
    eagine/build_info.hpp
    eagine/byteset.hpp
    eagine/callable_ref.hpp
    eagine/c_api/adapted_function.hpp
    eagine/c_api/constant.hpp
    eagine/c_api/function.hpp
    eagine/c_api/parameter_map.hpp
    eagine/c_api/result.hpp
    eagine/c_api_wrap.hpp
    eagine/cleanup_group.hpp
    eagine/compare.hpp
    eagine/compiler_info.hpp
    eagine/compression.hpp
    eagine/config/basic.hpp
    eagine/config/platform.hpp
    eagine/count.hpp
    eagine/c_str.hpp
    eagine/data_baking.hpp
    eagine/diagnostic.hpp
    eagine/double_buffer.hpp
    eagine/dynamic_library.hpp
    eagine/edit_distance.hpp
    eagine/embed.hpp
    eagine/enum_bitfield.hpp
    eagine/enum_class.hpp
    eagine/enum_log.hpp
    eagine/enum_map.hpp
    eagine/enum_span.hpp
    eagine/environment.hpp
    eagine/extract.hpp
    eagine/file_contents.hpp
    eagine/file_magic_number.hpp
    eagine/fixed_size_str.hpp
    eagine/flat_map.hpp
    eagine/flat_set.hpp
    eagine/from_string.hpp
    eagine/git_info.hpp
    eagine/handle.hpp
    eagine/hexdump.hpp
    eagine/identifier_ctr.hpp
    eagine/identifier.hpp
    eagine/identifier_t.hpp
    eagine/input_data.hpp
    eagine/instead_of.hpp
    eagine/int_constant.hpp
    eagine/integer_hash.hpp
    eagine/integer_range.hpp
    eagine/interface.hpp
    eagine/interleaved_call.hpp
    eagine/interop/valgrind.hpp
    eagine/is_within_limits.hpp
    eagine/iterator.hpp
    eagine/key_val_list.hpp
    eagine/logging/asio_backend.hpp
    eagine/logging/backend.hpp
    eagine/logging/config.hpp
    eagine/logging/entry.hpp
    eagine/logging/fwd.hpp
    eagine/logging/logger.hpp
    eagine/logging/null_backend.hpp
    eagine/logging/ostream_backend.hpp
    eagine/logging/proxy_backend.hpp
    eagine/logging/root_logger.hpp
    eagine/logging/root_logger_opts.hpp
    eagine/logging/severity.hpp
    eagine/logging/syslog_backend.hpp
    eagine/logging/type/build_info.hpp
    eagine/logging/type/exception.hpp
    eagine/logging/type/filesystem.hpp
    eagine/logging/type/program_args.hpp
    eagine/logging/type/yes_no_maybe.hpp
    eagine/main_ctx_fwd.hpp
    eagine/main_ctx.hpp
    eagine/main_ctx_object.hpp
    eagine/main_ctx_storage.hpp
    eagine/main_fwd.hpp
    eagine/main.hpp
    eagine/math/constants.hpp
    eagine/math/coordinates.hpp
    eagine/math/curve.hpp
    eagine/math/functions.hpp
    eagine/math/fwd.hpp
    eagine/math/interpolate.hpp
    eagine/math/intersection.hpp
    eagine/math/io.hpp
    eagine/math/matrix_ctr.hpp
    eagine/math/matrix_gauss.hpp
    eagine/math/matrix.hpp
    eagine/math/matrix_identity.hpp
    eagine/math/matrix_inverse.hpp
    eagine/math/matrix_looking_at.hpp
    eagine/math/matrix_orbiting.hpp
    eagine/math/matrix_ortho.hpp
    eagine/math/matrix_perspective.hpp
    eagine/math/matrix_reflection.hpp
    eagine/math/matrix_rotation.hpp
    eagine/math/matrix_scale.hpp
    eagine/math/matrix_translation.hpp
    eagine/math/primitives.hpp
    eagine/math/scalar.hpp
    eagine/math/sign.hpp
    eagine/math/tmat.hpp
    eagine/math/tvec.hpp
    eagine/math/vector.hpp
    eagine/maybe_unused.hpp
    eagine/mem_func_const.hpp
    eagine/memoized.hpp
    eagine/memory/address.hpp
    eagine/memory/align_alloc.hpp
    eagine/memory/aligned_block.hpp
    eagine/memory/align.hpp
    eagine/memory/alloc_arena.hpp
    eagine/memory/block.hpp
    eagine/memory/buffer.hpp
    eagine/memory/buffer_pool.hpp
    eagine/memory/byte_alloc.hpp
    eagine/memory/copy.hpp
    eagine/memory/c_realloc.hpp
    eagine/memory/default_alloc.hpp
    eagine/memory/fallback_alloc.hpp
    eagine/memory/flatten_fwd.hpp
    eagine/memory/flatten.hpp
    eagine/memory/fwd.hpp
    eagine/memory/null_alloc.hpp
    eagine/memory/null_ptr.hpp
    eagine/memory/object_storage.hpp
    eagine/memory/offset_ptr.hpp
    eagine/memory/offset_span.hpp
    eagine/memory/shared_alloc.hpp
    eagine/memory/span_algo.hpp
    eagine/memory/span.hpp
    eagine/memory/split_block.hpp
    eagine/memory/split_span.hpp
    eagine/memory/stack_alloc.hpp
    eagine/memory/std_alloc.hpp
    eagine/message_id.hpp
    eagine/mp_arithmetic.hpp
    eagine/mp_list.hpp
    eagine/mp_string.hpp
    eagine/mp_strings.hpp
    eagine/multi_byte_seq.hpp
    eagine/network_sorter.hpp
    eagine/nothing.hpp
    eagine/offset_ptr.hpp
    eagine/optional_ref.hpp
    eagine/os_info.hpp
    eagine/overloaded.hpp
    eagine/overwrite_guard.hpp
    eagine/power_supply_kind.hpp
    eagine/preprocessor.hpp
    eagine/process.hpp
    eagine/program_args.hpp
    eagine/progress/activity.hpp
    eagine/progress/backend.hpp
    eagine/progress_bar.hpp
    eagine/progress/default_backend.hpp
    eagine/progress/fwd.hpp
    eagine/progress/root_activity.hpp
    eagine/protected_member.hpp
    eagine/quantities.hpp
    eagine/random_bites.hpp
    eagine/random_bytes.hpp
    eagine/random_identifier.hpp
    eagine/range_types.hpp
    eagine/reflect/data_members.hpp
    eagine/reflect/decl_name.hpp
    eagine/reflect/enumerators.hpp
    eagine/reflect/map_data_members.hpp
    eagine/reflect/map_enumerators.hpp
    eagine/reflect/meta.hpp
    eagine/resources.hpp
    eagine/scope_exit.hpp
    eagine/selector.hpp
    eagine/serialize/block_sink.hpp
    eagine/serialize/block_source.hpp
    eagine/serialize/data_buffer.hpp
    eagine/serialize/data_sink.hpp
    eagine/serialize/data_source.hpp
    eagine/serialize/fast_backend.hpp
    eagine/serialize/fputils.hpp
    eagine/serialize/fwd.hpp
    eagine/serialize/istream_source.hpp
    eagine/serialize/ostream_sink.hpp
    eagine/serialize/packed_block_sink.hpp
    eagine/serialize/packed_block_source.hpp
    eagine/serialize/portable_backend.hpp
    eagine/serialize/read_backend.hpp
    eagine/serialize/read.hpp
    eagine/serialize/result.hpp
    eagine/serialize/size_and_data.hpp
    eagine/serialize/string_backend.hpp
    eagine/serialize/type/build_info.hpp
    eagine/serialize/type/compiler_info.hpp
    eagine/serialize/type/sudoku.hpp
    eagine/serialize/write_backend.hpp
    eagine/serialize/write.hpp
    eagine/signal_switch.hpp
    eagine/slow_exec.hpp
    eagine/sorting_network.hpp
    eagine/sorting_network_io.hpp
    eagine/span.hpp
    eagine/stacktrace.hpp
    eagine/str_format.hpp
    eagine/string_algo.hpp
    eagine/string_list.hpp
    eagine/string_path.hpp
    eagine/string_span.hpp
    eagine/struct_memory_block.hpp
    eagine/str_var_subst.hpp
    eagine/sudoku.hpp
    eagine/system_info.hpp
    eagine/tagged_quantity.hpp
    eagine/tags.hpp
    eagine/timeout.hpp
    eagine/tribool.hpp
    eagine/type_identity.hpp
    eagine/type_name.hpp
    eagine/types.hpp
    eagine/type_traits.hpp
    eagine/units/base_dim.hpp
    eagine/units/base_scaled_unit.hpp
    eagine/units/base_unit.hpp
    eagine/units/common.hpp
    eagine/units/custom_unit.hpp
    eagine/units/default.hpp
    eagine/units/detail.hpp
    eagine/units/dim/acceleration.hpp
    eagine/units/dim/angular_velocity.hpp
    eagine/units/dim/area.hpp
    eagine/units/dim/electrical_capacitance.hpp
    eagine/units/dim/electrical_conductance.hpp
    eagine/units/dim/electrical_resistance.hpp
    eagine/units/dim/electric_charge.hpp
    eagine/units/dim/electric_tension.hpp
    eagine/units/dim/energy.hpp
    eagine/units/dimension.hpp
    eagine/units/dimensionless.hpp
    eagine/units/dimensions.hpp
    eagine/units/dim/force.hpp
    eagine/units/dim/frequency.hpp
    eagine/units/dim/inductance.hpp
    eagine/units/dim/magnetic_field_strength.hpp
    eagine/units/dim/magnetic_flux.hpp
    eagine/units/dim/mass_density.hpp
    eagine/units/dim/momentum.hpp
    eagine/units/dim/power.hpp
    eagine/units/dim/pressure.hpp
    eagine/units/dim/radioactivity.hpp
    eagine/units/dim/velocity.hpp
    eagine/units/dim/volume.hpp
    eagine/units/fwd.hpp
    eagine/units/qty/si/time.hpp
    eagine/units/quantity.hpp
    eagine/units/scaled_unit.hpp
    eagine/units/scales.hpp
    eagine/units/strings.hpp
    eagine/units/traits.hpp
    eagine/units/unit.hpp
    eagine/units/unit/si/amount_of_substance.hpp
    eagine/units/unit/si/angle.hpp
    eagine/units/unit/si/common.hpp
    eagine/units/unit/si/electrical_capacitance.hpp
    eagine/units/unit/si/electrical_conductance.hpp
    eagine/units/unit/si/electrical_resistance.hpp
    eagine/units/unit/si/electric_charge.hpp
    eagine/units/unit/si/electric_current.hpp
    eagine/units/unit/si/electric_tension.hpp
    eagine/units/unit/si/energy.hpp
    eagine/units/unit/si/force.hpp
    eagine/units/unit/si/frequency.hpp
    eagine/units/unit/si.hpp
    eagine/units/unit/si/inductance.hpp
    eagine/units/unit/si/information.hpp
    eagine/units/unit/si/length.hpp
    eagine/units/unit/si/luminous_intensity.hpp
    eagine/units/unit/si/magnetic_field_strength.hpp
    eagine/units/unit/si/magnetic_flux.hpp
    eagine/units/unit/si/mass.hpp
    eagine/units/unit/si/power.hpp
    eagine/units/unit/si/pressure.hpp
    eagine/units/unit/si/radioactivity.hpp
    eagine/units/unit/si/solid_angle.hpp
    eagine/units/unit/si/temperature.hpp
    eagine/units/unit/si/time.hpp
    eagine/unreachable_reference.hpp
    eagine/url.hpp
    eagine/user_info.hpp
    eagine/valid_if/always.hpp
    eagine/valid_if/base.hpp
    eagine/valid_if/between.hpp
    eagine/valid_if/decl.hpp
    eagine/valid_if/filesystem.hpp
    eagine/valid_if/ge0_le1.hpp
    eagine/valid_if/ge0_lt1.hpp
    eagine/valid_if/greater_than.hpp
    eagine/valid_if/gt0_lt1.hpp
    eagine/valid_if.hpp
    eagine/valid_if/in_class.hpp
    eagine/valid_if/indicated.hpp
    eagine/valid_if/in_list.hpp
    eagine/valid_if/io.hpp
    eagine/valid_if/le_size_ge0.hpp
    eagine/valid_if/less_than.hpp
    eagine/valid_if/lt_size_ge0.hpp
    eagine/valid_if/lt_size.hpp
    eagine/valid_if/never.hpp
    eagine/valid_if/nonnegative.hpp
    eagine/valid_if/not_empty.hpp
    eagine/valid_if/not_equal.hpp
    eagine/valid_if/not_zero.hpp
    eagine/valid_if/one_of.hpp
    eagine/valid_if/positive.hpp
    eagine/valid_if/size_gt.hpp
    eagine/valid_if/within_limits.hpp
    eagine/value_tracker.hpp
    eagine/value_tree/empty.hpp
    eagine/value_tree/filesystem.hpp
    eagine/value_tree/implementation.hpp
    eagine/value_tree/interface.hpp
    eagine/value_tree/json.hpp
    eagine/value_tree/overlay.hpp
    eagine/value_tree/wrappers.hpp
    eagine/value_tree/yaml.hpp
    eagine/value_with_history.hpp
    eagine/vec_mat_traits.hpp
    eagine/vect/abs.hpp
    eagine/vect/axis.hpp
    eagine/vect/cast.hpp
    eagine/vect/compare.hpp
    eagine/vect/config.hpp
    eagine/vect/data_ary.hpp
    eagine/vect/data.hpp
    eagine/vect/data_vec.hpp
    eagine/vect/diff.hpp
    eagine/vect/esum.hpp
    eagine/vect/fill.hpp
    eagine/vect/from.hpp
    eagine/vect/fwd.hpp
    eagine/vect/hsum.hpp
    eagine/vect/sdiv.hpp
    eagine/vect/shuffle.hpp
    eagine/vect/sqrt.hpp
    eagine/vect/view.hpp
    eagine/watchdog.hpp
    eagine/workshop.hpp
    eagine/wrapping_container.hpp
)

set(PUB_INLS
    eagine/memory/alloc_arena.inl
    eagine/memory/c_realloc.inl
    eagine/memory/stack_alloc.inl
    eagine/multi_byte_seq.inl
    eagine/progress_bar.inl
    eagine/progress/root_activity.inl
)

set(LIB_INLS
    eagine/app_config.inl
    eagine/bindump.inl
    eagine/build_info.inl
    eagine/compiler_info.inl
    eagine/compression.inl
    eagine/edit_distance.inl
    eagine/file_contents.inl
    eagine/from_string.inl
    eagine/git_info.inl
    eagine/hexdump.inl
    eagine/identifier_ctr.inl
    eagine/input_data.inl
    eagine/logging/proxy_backend.inl
    eagine/logging/root_logger.inl
    eagine/main_ctx.inl
    eagine/main.inl
    eagine/memory/shared_alloc.inl
    eagine/os_info.inl
    eagine/random_identifier.inl
    eagine/resources.inl
    eagine/str_format.inl
    eagine/str_var_subst.inl
    eagine/system_info.inl
    eagine/type_name.inl
    eagine/url.inl
    eagine/user_info.inl
    eagine/value_tree/empty.inl
    eagine/value_tree/filesystem.inl
    eagine/value_tree/json.inl
    eagine/value_tree/overlay.inl
    eagine/value_tree/wrappers.inl
    eagine/value_tree/yaml.inl
    eagine/watchdog.inl
)

