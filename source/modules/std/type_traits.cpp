/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;
#include <type_traits>

export module std:type_traits;
export import std_private_type_traits_aligned_storage;
export import std_private_type_traits_extent;
export import std_private_type_traits_integral_constant;
export import std_private_type_traits_is_abstract;
export import std_private_type_traits_is_member_pointer;
export import std_private_type_traits_is_member_function_pointer;
export import std_private_type_traits_is_member_object_pointer;

export namespace std {
//------------------------------------------------------------------------------
using std::add_const;
using std::add_const_t;
using std::add_cv;
using std::add_cv_t;
using std::add_lvalue_reference;
using std::add_lvalue_reference_t;
using std::add_pointer;
using std::add_pointer_t;
using std::add_rvalue_reference;
using std::add_rvalue_reference_t;
using std::add_volatile;
using std::add_volatile_t;
using std::aligned_storage;
using std::aligned_storage_t;
using std::alignment_of;
using std::alignment_of_v;
using std::basic_common_reference;
using std::bool_constant;
using std::common_reference;
using std::common_reference_t;
using std::common_type;
using std::common_type_t;
using std::conditional;
using std::conditional_t;
using std::conjunction;
using std::conjunction_v;
using std::decay;
using std::decay_t;
using std::disjunction;
using std::disjunction_v;
using std::enable_if;
using std::enable_if_t;
using std::extent;
using std::extent_v;
using std::false_type;
using std::has_unique_object_representations;
using std::has_unique_object_representations_v;
using std::has_virtual_destructor;
using std::has_virtual_destructor_v;
using std::integral_constant;
using std::invoke_result;
using std::invoke_result_t;
using std::is_abstract;
using std::is_abstract_v;
using std::is_aggregate;
using std::is_aggregate_v;
using std::is_arithmetic;
using std::is_arithmetic_v;
using std::is_array;
using std::is_array_v;
using std::is_assignable;
using std::is_assignable_v;
using std::is_base_of;
using std::is_base_of_v;
using std::is_bounded_array;
using std::is_bounded_array_v;
using std::is_class;
using std::is_class_v;
using std::is_compound;
using std::is_compound_v;
using std::is_const;
using std::is_const_v;
using std::is_constant_evaluated;
using std::is_constructible;
using std::is_constructible_v;
using std::is_convertible;
using std::is_convertible_v;
using std::is_copy_assignable;
using std::is_copy_assignable_v;
using std::is_copy_constructible;
using std::is_copy_constructible_v;
using std::is_default_constructible;
using std::is_default_constructible_v;
using std::is_destructible;
using std::is_destructible_v;
using std::is_empty;
using std::is_empty_v;
using std::is_enum;
using std::is_enum_v;
using std::is_final;
using std::is_final_v;
using std::is_floating_point;
using std::is_floating_point_v;
using std::is_function;
using std::is_function_v;
using std::is_fundamental;
using std::is_fundamental_v;
using std::is_integral;
using std::is_integral_v;
using std::is_invocable;
using std::is_invocable_r_v;
using std::is_invocable_v;
using std::is_lvalue_reference;
using std::is_lvalue_reference_v;
using std::is_member_function_pointer;
using std::is_member_function_pointer_v;
using std::is_member_object_pointer;
using std::is_member_object_pointer_v;
using std::is_member_pointer;
using std::is_member_pointer_v;
using std::is_move_assignable;
using std::is_move_assignable_v;
using std::is_move_constructible;
using std::is_move_constructible_v;
using std::is_nothrow_assignable;
using std::is_nothrow_assignable_v;
using std::is_nothrow_constructible;
using std::is_nothrow_constructible_v;
using std::is_nothrow_convertible;
using std::is_nothrow_convertible_v;
using std::is_nothrow_copy_assignable;
using std::is_nothrow_copy_assignable_v;
using std::is_nothrow_copy_constructible;
using std::is_nothrow_copy_constructible_v;
using std::is_nothrow_default_constructible;
using std::is_nothrow_default_constructible_v;
using std::is_nothrow_destructible;
using std::is_nothrow_destructible_v;
using std::is_nothrow_invocable;
using std::is_nothrow_invocable_r_v;
using std::is_nothrow_invocable_v;
using std::is_nothrow_move_assignable;
using std::is_nothrow_move_assignable_v;
using std::is_nothrow_move_constructible;
using std::is_nothrow_move_constructible_v;
using std::is_nothrow_swappable;
using std::is_nothrow_swappable_v;
using std::is_nothrow_swappable_with;
using std::is_nothrow_swappable_with_v;
using std::is_null_pointer;
using std::is_null_pointer_v;
using std::is_object;
using std::is_object_v;
using std::is_pointer;
using std::is_pointer_v;
using std::is_polymorphic;
using std::is_polymorphic_v;
using std::is_reference;
using std::is_reference_v;
using std::is_rvalue_reference;
using std::is_rvalue_reference_v;
using std::is_same;
using std::is_same_v;
using std::is_scalar;
using std::is_scalar_v;
using std::is_scoped_enum;
using std::is_scoped_enum_v;
using std::is_signed;
using std::is_signed_v;
using std::is_standard_layout;
using std::is_standard_layout_v;
using std::is_swappable;
using std::is_swappable_v;
using std::is_swappable_with;
using std::is_swappable_with_v;
using std::is_trivial;
using std::is_trivial_v;
using std::is_trivially_assignable;
using std::is_trivially_assignable_v;
using std::is_trivially_constructible;
using std::is_trivially_constructible_v;
using std::is_trivially_copy_assignable;
using std::is_trivially_copy_assignable_v;
using std::is_trivially_copy_constructible;
using std::is_trivially_copy_constructible_v;
using std::is_trivially_copyable;
using std::is_trivially_copyable_v;
using std::is_trivially_default_constructible;
using std::is_trivially_default_constructible_v;
using std::is_trivially_destructible;
using std::is_trivially_destructible_v;
using std::is_trivially_move_assignable;
using std::is_trivially_move_assignable_v;
using std::is_trivially_move_constructible;
using std::is_trivially_move_constructible_v;
using std::is_unbounded_array;
using std::is_unbounded_array_v;
using std::is_union;
using std::is_union_v;
using std::is_unsigned;
using std::is_unsigned_v;
using std::is_void;
using std::is_void_v;
using std::is_volatile;
using std::is_volatile_v;
using std::make_signed;
using std::make_signed_t;
using std::make_unsigned;
using std::make_unsigned_t;
using std::negation;
using std::negation_v;
using std::rank;
using std::rank_v;
using std::remove_all_extents;
using std::remove_all_extents_t;
using std::remove_const;
using std::remove_const_t;
using std::remove_cv;
using std::remove_cv_t;
using std::remove_cvref;
using std::remove_cvref_t;
using std::remove_extent;
using std::remove_extent_t;
using std::remove_pointer;
using std::remove_pointer_t;
using std::remove_reference;
using std::remove_reference_t;
using std::remove_volatile;
using std::remove_volatile_t;
using std::true_type;
using std::type_identity;
using std::type_identity_t;
using std::underlying_type;
using std::underlying_type_t;
using std::unwrap_ref_decay;
using std::unwrap_ref_decay_t;
using std::unwrap_reference;
using std::unwrap_reference_t;
using std::void_t;
//------------------------------------------------------------------------------
} // namespace std