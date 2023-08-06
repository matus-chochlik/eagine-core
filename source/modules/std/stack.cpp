/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;
#include <stack>

export module std:stack;
export import std_stack;

export namespace std {
//------------------------------------------------------------------------------
using std::stack;
using std::swap;
using std::uses_allocator;

using std::operator==;
using std::operator!=;
using std::operator<;
using std::operator>;
using std::operator<=;
using std::operator>=;
using std::operator<=>;
//------------------------------------------------------------------------------
} // namespace std
