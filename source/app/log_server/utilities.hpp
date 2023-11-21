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
auto padded_to(std::size_t, const std::string&) noexcept -> std::string;
//------------------------------------------------------------------------------
} // namespace eagine::logs

#endif

