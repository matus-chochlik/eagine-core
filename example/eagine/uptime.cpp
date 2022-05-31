/// @example eagine/uptime.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/console/console.hpp>
#include <eagine/main.hpp>
#include <eagine/system_info.hpp>
#include <iostream>

namespace eagine {

auto main(main_ctx& ctx) -> int {

    const string_view n_a("N/A");
    auto& sys = ctx.system();
    const auto sys_cio{
      ctx.cio().print(EAGINE_ID(system), "system stats").to_be_continued()};
    sys_cio.print("short average load: ${value}")
      .arg(
        EAGINE_ID(value),
        EAGINE_ID(Ratio),
        either_or(sys.short_average_load(), n_a));
    sys_cio.print("long average load: ${value}")
      .arg(
        EAGINE_ID(value),
        EAGINE_ID(Ratio),
        either_or(sys.long_average_load(), n_a));
    sys_cio.print("uptime: ${value}").arg(EAGINE_ID(value), sys.uptime());

    return 0;
}

} // namespace eagine
