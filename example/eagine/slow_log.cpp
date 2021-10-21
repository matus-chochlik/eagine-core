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
#include <iostream>
#include <thread>

namespace eagine {

auto main(main_ctx& ctx) -> int {

    span_size_t repeats = 10;
    ctx.args().find("--count").parse_next(repeats, ctx.log().error_stream());

    const auto main_act = ctx.progress().activity("Counting", repeats);
    for(const auto i : integer_range(repeats)) {
        ctx.log()
          .info("cycle ${i} of ${count}")
          .arg(EAGINE_ID(i), i)
          .arg(EAGINE_ID(count), repeats);
        main_act.update_progress(i);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}

} // namespace eagine
