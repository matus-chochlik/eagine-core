/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;
#include <cstddef>

export module std:cstddef;

export import _Builtin_stddef_max_align_t;

export namespace std {
//------------------------------------------------------------------------------
using std::byte;
using std::max_align_t;
using std::nullptr_t;
using std::ptrdiff_t;
using std::size_t;

using std::operator<<=;
using std::operator<<;
using std::operator>>=;
using std::operator>>;
using std::operator|=;
using std::operator|;
using std::operator&=;
using std::operator&;
using std::operator^=;
using std::operator^;
using std::operator~;
using std::operator<<;
using std::to_integer;
//------------------------------------------------------------------------------
} // namespace std
