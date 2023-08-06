/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;
#include <tuple>

export module std:tuple;

export namespace std {
//------------------------------------------------------------------------------
using std::apply;
using std::basic_common_reference;
using std::common_type;
using std::forward_as_tuple;
using std::get;
using std::make_from_tuple;
using std::make_tuple;
using std::tie;
using std::tuple;
using std::tuple_cat;
using std::tuple_element;
using std::tuple_element_t;
using std::tuple_size;
using std::tuple_size_v;
using std::uses_allocator;

using std::operator==;
using std::operator<=>;
//------------------------------------------------------------------------------
} // namespace std
