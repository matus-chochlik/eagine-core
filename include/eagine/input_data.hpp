/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#ifndef EAGINE_INPUT_DATA_HPP
#define EAGINE_INPUT_DATA_HPP

#include "config/basic.hpp"
#include "memory/buffer.hpp"
#include "string_span.hpp"
#include <iosfwd>
#include <vector>

namespace eagine {

auto read_stream_data(std::istream&, memory::buffer& dest) noexcept -> bool;
auto read_file_data(const string_view path, memory::buffer& dest) noexcept
  -> bool;

} // namespace eagine

#if !EAGINE_CORE_LIBRARY || defined(EAGINE_IMPLEMENTING_CORE_LIBRARY)
#include <eagine/input_data.inl>
#endif

#endif // EAGINE_INPUT_DATA_HPP
