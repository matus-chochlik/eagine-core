/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;
#include <typeinfo>

export module std:typeinfo;
export import std_typeinfo;

export namespace std {
//------------------------------------------------------------------------------
using std::bad_cast;
using std::bad_typeid;
using std::type_info;
//------------------------------------------------------------------------------
} // namespace std
