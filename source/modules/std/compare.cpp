/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;
#include <compare>

export module std:compare;

export namespace std {
//------------------------------------------------------------------------------
using std::common_comparison_category;
using std::common_comparison_category_t;
using std::compare_three_way;
using std::compare_three_way_result;
using std::compare_three_way_result_t;
using std::is_eq;
using std::is_gt;
using std::is_gteq;
using std::is_lt;
using std::is_lteq;
using std::is_neq;
using std::partial_ordering;
using std::strong_ordering;
using std::three_way_comparable;
using std::three_way_comparable_with;
using std::weak_ordering;
//------------------------------------------------------------------------------
} // namespace std
