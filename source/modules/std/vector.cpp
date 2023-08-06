/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;
#include <vector>

export module std:vector;
export import std_vector;

export namespace std {
//------------------------------------------------------------------------------
using std::erase;
using std::erase_if;
using std::swap;
using std::vector;
using std::operator==;
using std::operator<=>;
namespace pmr {
using std::pmr::vector;
}
//------------------------------------------------------------------------------
} // namespace std
