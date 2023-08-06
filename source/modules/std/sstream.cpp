/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;
#include <sstream>

export module std:sstream;
export import std_sstream;

export namespace std {
//------------------------------------------------------------------------------
using std::basic_istringstream;
using std::basic_ostringstream;
using std::basic_stringbuf;
using std::basic_stringstream;
using std::istringstream;
using std::ostringstream;
using std::stringbuf;
using std::stringstream;
//------------------------------------------------------------------------------
} // namespace std
