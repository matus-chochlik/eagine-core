/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;
#include <fstream>

export module std:fstream;
export import std_fstream;

export namespace std {
//------------------------------------------------------------------------------
using std::basic_filebuf;
using std::basic_fstream;
using std::basic_ifstream;
using std::basic_ofstream;
using std::filebuf;
using std::fstream;
using std::ifstream;
using std::ofstream;
using std::swap;
//------------------------------------------------------------------------------
} // namespace std
