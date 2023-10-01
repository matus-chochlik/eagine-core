/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module std:system_error;
export import <system_error>;

export namespace std {
//------------------------------------------------------------------------------
using std::errc;
using std::error_category;
using std::error_code;
using std::error_condition;
using std::generic_category;
using std::hash;
using std::is_error_code_enum;
using std::is_error_code_enum_v;
using std::is_error_condition_enum;
using std::is_error_condition_enum_v;
using std::make_error_code;
using std::make_error_condition;
using std::system_category;
using std::system_error;

using std::operator<<;
using std::operator==;
using std::operator<=>;
//------------------------------------------------------------------------------
} // namespace std
