/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;
#include <cstring>

export module std:cstring;

export namespace std {
//------------------------------------------------------------------------------
using std::memcmp;
using std::memcpy;
using std::memmove;
using std::memset;
using std::strcmp;
using std::strlen;
using std::strncmp;
//------------------------------------------------------------------------------
} // namespace std
