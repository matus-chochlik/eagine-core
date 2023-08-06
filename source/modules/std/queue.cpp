/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;
#include <queue>

export module std:queue;
export import std_queue;

export namespace std {
//------------------------------------------------------------------------------
using std::queue;
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
