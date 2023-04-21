/// @example eagine/uptime.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import std;

namespace eagine {

auto main(main_ctx& ctx) -> int {

    const string_view n_a("N/A");
    auto& sys = ctx.system();
    const auto sys_cio{
      ctx.cio().print("system", "system stats").to_be_continued()};
    sys_cio.print("short average load: ${value}")
      .arg("value", "Ratio", either_or(sys.short_average_load(), n_a));
    sys_cio.print("long average load: ${value}")
      .arg("value", "Ratio", either_or(sys.long_average_load(), n_a));
    sys_cio.print("uptime: ${value}").arg("value", sys.uptime());

    return 0;
}

} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

