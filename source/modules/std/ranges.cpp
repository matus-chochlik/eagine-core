/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module std:ranges;
export import <ranges>;

export namespace std {
namespace ranges {
//------------------------------------------------------------------------------
using std::ranges::basic_istream_view;
using std::ranges::bidirectional_range;
using std::ranges::borrowed_iterator_t;
using std::ranges::borrowed_range;
using std::ranges::borrowed_subrange_t;
using std::ranges::common_range;
using std::ranges::contiguous_range;
using std::ranges::dangling;
using std::ranges::disable_sized_range;
using std::ranges::empty_view;
using std::ranges::enable_borrowed_range;
using std::ranges::enable_view;
using std::ranges::forward_range;
using std::ranges::get;
using std::ranges::input_range;
using std::ranges::iota_view;
using std::ranges::istream_view;
using std::ranges::iterator_t;
using std::ranges::output_range;
using std::ranges::random_access_range;
using std::ranges::range;
using std::ranges::range_difference_t;
using std::ranges::range_reference_t;
using std::ranges::range_rvalue_reference_t;
using std::ranges::range_size_t;
using std::ranges::range_value_t;
using std::ranges::sentinel_t;
using std::ranges::single_view;
using std::ranges::sized_range;
using std::ranges::subrange;
using std::ranges::subrange_kind;
using std::ranges::view;
using std::ranges::view_base;
using std::ranges::view_interface;
using std::ranges::viewable_range;
namespace views {
using std::ranges::views::empty;
using std::ranges::views::iota;
using std::ranges::views::single;
} // namespace views
//------------------------------------------------------------------------------
} // namespace ranges
using std::ranges::get;
namespace views = ranges::views;
//------------------------------------------------------------------------------
} // namespace std

