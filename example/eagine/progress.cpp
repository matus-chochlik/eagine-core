/// @example eagine/progress.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/integer_range.hpp>
#include <eagine/logging/logger.hpp>
#include <eagine/main.hpp>
#include <eagine/main_ctx.hpp>
#include <eagine/progress/activity.hpp>

namespace eagine {
//------------------------------------------------------------------------------
auto main(main_ctx& ctx) -> int {
    const span_size_t n = 1000000;

    const auto callback = [&]() {
        ctx.log().info("Progress callback called");
    };
    set_progress_update_callback(
      ctx, {construct_from, callback}, std::chrono::milliseconds{100});

    const auto act = ctx.progress().activity("Counting", n);

    for(const auto i : integer_range(n)) {
        act.update_progress(i);
    }

    return 0;
}
//------------------------------------------------------------------------------
} // namespace eagine
