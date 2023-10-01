/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module std:charconv;
export import <charconv>;

export namespace std {
//------------------------------------------------------------------------------
using std::chars_format;
using std::from_chars;
using std::from_chars_result;
using std::to_chars;
using std::to_chars_result;

using std::operator&;
using std::operator&=;
using std::operator^;
using std::operator^=;
using std::operator|;
using std::operator|=;
using std::operator~;
//------------------------------------------------------------------------------
} // namespace std
