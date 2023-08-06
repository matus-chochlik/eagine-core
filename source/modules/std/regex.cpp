/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;
#include <regex>

export module std:regex;
export import std_regex;

export namespace std {
//------------------------------------------------------------------------------
namespace regex_constants {
using std::regex_constants::error_type;
using std::regex_constants::match_flag_type;
using std::regex_constants::syntax_option_type;

// regex_constants is a bitmask type.
// [bitmask.types] specified operators
using std::regex_constants::operator&;
using std::regex_constants::operator&=;
using std::regex_constants::operator^;
using std::regex_constants::operator^=;
using std::regex_constants::operator|;
using std::regex_constants::operator|=;
using std::regex_constants::operator~;

} // namespace regex_constants

using std::basic_regex;
using std::cmatch;
using std::cregex_iterator;
using std::cregex_token_iterator;
using std::match_results;
using std::regex;
using std::regex_error;
using std::regex_iterator;
using std::regex_match;
using std::regex_replace;
using std::regex_search;
using std::regex_token_iterator;
using std::regex_traits;
using std::smatch;
using std::sregex_iterator;
using std::sregex_token_iterator;
using std::sub_match;
using std::swap;
//------------------------------------------------------------------------------
} // namespace std
