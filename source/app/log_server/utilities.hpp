/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#ifndef EAGINE_CORE_LOG_SERVER_UTILITIES_HPP
#define EAGINE_CORE_LOG_SERVER_UTILITIES_HPP

import eagine.core;
import std;

namespace eagine::logs {
//------------------------------------------------------------------------------
auto format_reltime(std::chrono::microseconds) noexcept -> std::string;
//------------------------------------------------------------------------------
struct string_padded_to {
    string_view str;
    span_size_t before{0};
    span_size_t after{0};

    friend auto operator<<(
      std::ostream& output,
      const string_padded_to& s) noexcept -> std::ostream&;
};
//------------------------------------------------------------------------------
auto padded_to(std::size_t, const string_view) noexcept -> string_padded_to;
//------------------------------------------------------------------------------
} // namespace eagine::logs

#endif

