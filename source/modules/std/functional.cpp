/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;
#include <functional>

export module std:functional;
export import std_private_functional_function;

export namespace std {
//------------------------------------------------------------------------------
using std::cref;
using std::greater;
using std::invoke;
using std::invoke_r;
using std::less;
using std::ref;
using std::reference_wrapper;
//------------------------------------------------------------------------------
} // namespace std
