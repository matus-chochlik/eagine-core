/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;
#include <array>

export module std:array;
export import std_array;

export namespace std {
//------------------------------------------------------------------------------
using std::array;
using std::to_array;

using std::operator==;
//------------------------------------------------------------------------------
} // namespace std
