/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module std:memory;
export import <memory>;
export import <new>;

export namespace std {
//------------------------------------------------------------------------------
using std::addressof;
using std::align;
using std::allocator;
using std::allocator_arg;
using std::allocator_arg_t;
using std::allocator_traits;
using std::assume_aligned;
using std::bad_alloc;
using std::bad_weak_ptr;
using std::const_pointer_cast;
using std::construct_at;
using std::default_delete;
using std::destroy;
using std::destroy_at;
using std::destroy_n;
using std::dynamic_pointer_cast;
using std::enable_shared_from_this;
using std::hash;
using std::launder;
using std::make_obj_using_allocator;
using std::make_shared;
using std::make_unique;
using std::pointer_traits;
using std::reinterpret_pointer_cast;
using std::shared_ptr;
using std::static_pointer_cast;
using std::to_address;
using std::uninitialized_construct_using_allocator;
using std::uninitialized_copy;
using std::uninitialized_copy_n;
using std::uninitialized_default_construct;
using std::uninitialized_default_construct_n;
using std::uninitialized_fill;
using std::uninitialized_fill_n;
using std::uninitialized_move;
using std::uninitialized_move_n;
using std::uninitialized_value_construct;
using std::uninitialized_value_construct_n;
using std::unique_ptr;
using std::uses_allocator;
using std::uses_allocator_construction_args;
using std::uses_allocator_v;
using std::weak_ptr;

using std::operator==;
using std::operator<;
using std::operator>;
using std::operator<=;
using std::operator>=;
using std::operator<=>;
using std::operator<<;
//------------------------------------------------------------------------------
} // namespace std
