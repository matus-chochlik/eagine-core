/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;
#include <span>

export module std:span;
export import std_span;
export import std_private_span_span_fwd;

export namespace std {
//------------------------------------------------------------------------------
using std::as_bytes;
using std::as_writable_bytes;
using std::dynamic_extent;
using std::span;
//------------------------------------------------------------------------------
} // namespace std
