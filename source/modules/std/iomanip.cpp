/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;
#include <iomanip>

export module std:iomanip;

export namespace std {
//------------------------------------------------------------------------------
using std::dec;
using std::get_money;
using std::get_time;
using std::hex;
using std::put_money;
using std::put_time;
using std::quoted;
using std::resetiosflags;
using std::setbase;
using std::setfill;
using std::setiosflags;
using std::setprecision;
using std::setw;

using std::operator<<;
//------------------------------------------------------------------------------
} // namespace std
