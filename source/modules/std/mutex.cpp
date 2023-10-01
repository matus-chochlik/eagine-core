/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module std:mutex;
export import <mutex>;
export import <shared_mutex>;

export namespace std {
//------------------------------------------------------------------------------
using std::adopt_lock;
using std::adopt_lock_t;
using std::call_once;
using std::defer_lock;
using std::defer_lock_t;
using std::lock;
using std::lock_guard;
using std::mutex;
using std::once_flag;
using std::recursive_mutex;
using std::recursive_timed_mutex;
using std::scoped_lock;
using std::shared_lock;
using std::shared_mutex;
using std::swap;
using std::timed_mutex;
using std::try_lock;
using std::try_to_lock;
using std::try_to_lock_t;
using std::unique_lock;
//------------------------------------------------------------------------------
} // namespace std
