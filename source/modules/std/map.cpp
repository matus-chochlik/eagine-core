/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module std:map;
export import <map>;

export namespace std {
//------------------------------------------------------------------------------
using std::erase_if;
using std::less;
using std::map;
using std::multimap;
using std::swap;

namespace pmr {
using std::pmr::map;
using std::pmr::multimap;
} // namespace pmr
//------------------------------------------------------------------------------
} // namespace std
