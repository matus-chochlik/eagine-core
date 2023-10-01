/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module std:string;
export import <string>;

export namespace std {
//------------------------------------------------------------------------------
using std::basic_string;
using std::char_traits;
using std::erase;
using std::erase_if;
using std::getline;
using std::string;

using std::operator<=>;
using std::operator==;
//------------------------------------------------------------------------------
} // namespace std
