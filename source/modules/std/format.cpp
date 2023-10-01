/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module std:format;
export import <format>;

export namespace std {
//------------------------------------------------------------------------------
using std::basic_format_arg;
using std::basic_format_args;
using std::basic_format_context;
using std::basic_format_string;
using std::format;
using std::format_args;
using std::format_context;
using std::format_error;
using std::format_kind;
using std::format_string;
using std::format_to;
using std::format_to_n;
using std::format_to_n_result;
using std::formattable;
using std::formatted_size;
using std::formatter;
using std::make_format_args;
using std::range_format;
using std::range_formatter;
using std::vformat;
using std::vformat_to;
using std::visit_format_arg;
//------------------------------------------------------------------------------
} // namespace std
