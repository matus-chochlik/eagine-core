/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module std:set;
export import <set>;

export namespace std {
//------------------------------------------------------------------------------
using std::erase_if;
using std::less;
using std::multiset;
using std::set;
using std::swap;

using std::operator==;
using std::operator<=>;
namespace pmr {
using std::pmr::multiset;
using std::pmr::set;
} // namespace pmr
//------------------------------------------------------------------------------
} // namespace std
