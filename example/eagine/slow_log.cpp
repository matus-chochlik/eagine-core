/// @example eagine/slow_log.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/integer_range.hpp>
#include <eagine/logging/logger.hpp>
#include <eagine/main.hpp>
#include <eagine/progress/activity.hpp>
#include <eagine/reflect/enumerators.hpp>
#include <thread>

namespace eagine {

auto main(main_ctx& ctx) -> int {

    span_size_t repeats = 10;
    auto severity = log_event_severity::info;
    ctx.args().find("--count").parse_next(repeats, ctx.log().error_stream());
    ctx.args().find("--severity").parse_next(severity, ctx.log().error_stream());

    const auto main_act =
      ctx.progress().activity("Counting ${current} / ${total}", repeats);
    for(const auto i : integer_range(repeats)) {
        ctx.log()
          .log(severity, "cycle ${i} of ${count}")
          .arg(EAGINE_ID(i), i)
          .arg(EAGINE_ID(count), repeats);
        main_act.update_progress(i);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}

} // namespace eagine
