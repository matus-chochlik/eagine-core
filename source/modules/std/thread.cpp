/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;
#include <thread>

export module std:thread;
export import std_thread;
export import std_private_threading_support;
export import std_private_thread_thread;
export import std_private_thread_this_thread;

export namespace std {
//------------------------------------------------------------------------------
using std::hash;
using std::swap;
using std::thread;

using std::operator==;
using std::operator<=>;
using std::operator<<;

namespace this_thread {
using std::this_thread::get_id;

using std::this_thread::sleep_for;
using std::this_thread::sleep_until;
using std::this_thread::yield;
} // namespace this_thread
//------------------------------------------------------------------------------
} // namespace std