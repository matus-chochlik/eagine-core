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
auto padded_to(std::size_t, std::string_view) noexcept -> std::string;
auto padded_to(std::size_t, identifier) noexcept -> std::string;
//------------------------------------------------------------------------------
// tiling
//------------------------------------------------------------------------------
export class tiling_state {
public:
    void mark_cell_done(int x, int y, int w, int h) noexcept;

    auto pixel_coef(int j, int i, int w, int h) const noexcept -> float;
    auto pixel_glyph(int j, int i, int w, int h) const noexcept -> string_view;

private:
    std::vector<bool> _bits;
    int _w{0};
    int _h{0};
};
//------------------------------------------------------------------------------
} // namespace eagine::logs

