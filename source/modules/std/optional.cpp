/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;
#include <optional>

export module std:optional;
export import std_optional;

export namespace std {
//------------------------------------------------------------------------------
using std::bad_optional_access;
using std::make_optional;
using std::nullopt;
using std::nullopt_t;
using std::optional;
//------------------------------------------------------------------------------
} // namespace std
