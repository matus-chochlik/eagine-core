/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.log_server:utilities;

import eagine.core;
import std;
import :interfaces;

namespace eagine::logs {
//------------------------------------------------------------------------------
auto format_reltime(std::chrono::microseconds) noexcept -> std::string;
auto format_reltime_s(float_seconds) noexcept -> std::string;
auto format_instance(std::uint64_t, std::string&) noexcept -> std::string&;
//------------------------------------------------------------------------------
// message formatter
//------------------------------------------------------------------------------
class message_formatter {
public:
    auto format(const message_info::arg_info&, bool short_value) noexcept
      -> std::string;
    auto format(const message_info&) noexcept -> std::string;

private:
    float_seconds _curr_msg_time{};
    float_seconds _main_prgrs_start{};
    bool _main_prgrs_started{false};
};
//------------------------------------------------------------------------------
// padded streamable string
//------------------------------------------------------------------------------
struct string_padded_to {
    const std::string_view str;
    const std::size_t length{0};

    friend auto operator<<(
      std::ostream& output,
      const string_padded_to& s) noexcept -> std::ostream&;
};
auto padded_to(std::size_t, std::string_view) noexcept -> string_padded_to;
//------------------------------------------------------------------------------
struct identifier_padded_to {
    const identifier id;
    const std::size_t length{0};

    friend auto operator<<(
      std::ostream& output,
      const identifier_padded_to& s) noexcept -> std::ostream&;
};
auto padded_to(std::size_t, identifier) noexcept -> identifier_padded_to;
//------------------------------------------------------------------------------
} // namespace eagine::logs

