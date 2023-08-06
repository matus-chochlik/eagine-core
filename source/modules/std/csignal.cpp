/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;
#include <csignal>

export module std:csignal;

export namespace std {
//------------------------------------------------------------------------------
using std::raise;
using std::sig_atomic_t;
using std::signal;
//------------------------------------------------------------------------------
} // namespace std
