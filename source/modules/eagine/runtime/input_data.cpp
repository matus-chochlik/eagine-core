/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.runtime:input_data;

import eagine.core.types;
import eagine.core.memory;
import <iosfwd>;

namespace eagine {

auto read_stream_data(std::istream&, memory::buffer& dest) noexcept -> bool;
auto read_file_data(const string_view path, memory::buffer& dest) noexcept
  -> bool;

} // namespace eagine

