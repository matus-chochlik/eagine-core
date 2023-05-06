/// @example eagine/slow_log.cpp
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

    span_size_t repeats = 10;
    auto severity = log_event_severity::info;
    ctx.config().fetch("count", repeats");
	ctx.config().fetch("severity", severity");

    const auto main_act =
      ctx.progress().activity("Counting ${current} / ${total}", repeats);
    for(const auto i : integer_range(repeats)) {
        ctx.log()
          .log(severity, "cycle ${i} of ${count}")
          .arg("i", i)
          .arg("count", repeats);
        main_act.update_progress(i);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}

} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

