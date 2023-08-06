/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;
#include <ostream>

export module std:ostream;

export namespace std {
//------------------------------------------------------------------------------
using std::basic_ostream;
using std::endl;
using std::ends;
using std::flush;
using std::ostream;

using std::operator<<;
//------------------------------------------------------------------------------
} // namespace std
