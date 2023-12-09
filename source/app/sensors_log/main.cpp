/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
import std;
import eagine.core;

namespace eagine {
//------------------------------------------------------------------------------
void do_log(main_ctx& ctx) noexcept {
    auto& log{ctx.log()};
    auto& sys{ctx.system()};

    log.log_stat("CPU load (short: ${short}, long: ${long})")
      .tag("cpuLoad")
      .arg("short", "ratio", sys.short_average_load().or_default())
      .arg("long", "ratio", sys.long_average_load().or_default());

    log.log_stat("Memory usage (free: ${free}, total: ${total})")
      .tag("ramUsage")
      .arg("free", "ByteSize", sys.free_ram_size().or_default())
      .arg("total", "ByteSize", sys.total_ram_size().or_default());

    log.log_stat("Swap usage (free: ${free}, total: ${total})")
      .tag("swapUsage")
      .arg("free", "ByteSize", sys.free_swap_size().or_default())
      .arg("total", "ByteSize", sys.total_swap_size().or_default());

    const auto [tk_min, tk_max]{sys.temperature_min_max()};
    if(tk_min and tk_max) {
        log.log_stat("Thermal info: (min: ${min}, max: ${max})")
          .tag("thermal")
          .arg("min", "DegreeK", tk_min->value())
          .arg("max", "DegreeK", tk_max->value());
    }
}
//------------------------------------------------------------------------------
auto main(main_ctx& ctx) -> int {
    try {
        signal_switch interrupted;
        const application_reconfig_value<std::chrono::seconds> interval{
          ctx.config(), "sensors_log.interval", std::chrono::minutes(1)};

        while(not interrupted) {
            do_log(ctx);
            std::this_thread::sleep_for(interval.value());
        }
        return 0;
    } catch(std::exception& error) {
        ctx.log().error("sensor log error: ${what}").arg("what", error.what());
    }
    return 1;
}
//------------------------------------------------------------------------------
} // namespace eagine
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagine::main_ctx_options options{.app_id = "SensorsLog"};
    return eagine::main_impl(argc, argv, options, eagine::main);
}
//------------------------------------------------------------------------------

