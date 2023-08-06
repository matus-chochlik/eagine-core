/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;
#include <coroutine>

export module std:coroutine;
export import std_private_coroutine_coroutine_handle;
export import std_private_coroutine_coroutine_traits;
export import std_private_coroutine_trivial_awaitables;

export namespace std {
//------------------------------------------------------------------------------
using std::coroutine_handle;
using std::coroutine_traits;
using std::noop_coroutine;
using std::noop_coroutine_handle;
using std::noop_coroutine_promise;
using std::suspend_always;
using std::suspend_never;

using std::operator==;
using std::operator<=>;
//------------------------------------------------------------------------------
} // namespace std
