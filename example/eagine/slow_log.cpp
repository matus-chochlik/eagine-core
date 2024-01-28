/// @example eagine/slow_log.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import std;

namespace eagine {

auto main(main_ctx& ctx) -> int {
    std::default_random_engine random{std::random_device{}()};
    const auto sleep_interval{[&]() {
        using U = std::chrono::milliseconds;
        return U{std::uniform_int_distribution<U::rep>{200, 1800}(random)};
    }};

    span_size_t repeats = 10;
    auto severity = log_event_severity::info;
    ctx.config().fetch("count", repeats);
    ctx.config().fetch("severity", severity);

    ctx.log().declare_state("running", "start", "finish");
    ctx.log().info("starting").tag("start");

    const auto main_act =
      ctx.progress().activity("Counting ${current} / ${total}", repeats);
    for(const auto i : integer_range(repeats)) {
        ctx.log()
          .log(severity, "cycle ${i} of ${count}")
          .arg("i", i)
          .arg("count", repeats);
        main_act.update_progress(i);
        std::this_thread::sleep_for(sleep_interval());
    }
    ctx.log().info("finishing").tag("finish");

    return 0;
}

} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

