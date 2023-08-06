/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;
#include <string_view>

export module std:string_view;
export import std_string_view;

export namespace std {
//------------------------------------------------------------------------------
using std::basic_string_view;
using std::string_view;
using std::u8string_view;

using std::operator==;
using std::operator<=>;
using std::operator<<;

inline namespace literals {
inline namespace string_view_literals {
using std::literals::string_view_literals::operator""sv;
} // namespace string_view_literals
} // namespace literals
//------------------------------------------------------------------------------
} // namespace std
