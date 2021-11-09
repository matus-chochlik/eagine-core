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
#include <eagine/progress/backend.hpp>
#include <iostream>

namespace eagine {
//------------------------------------------------------------------------------
class example_observer final : public progress_observer {
public:
    void activity_begun(
      const activity_progress_id_t parent_id,
      const activity_progress_id_t activity_id,
      const string_view title,
      const span_size_t total_steps) noexcept final {
        std::cout << "activity " << parent_id << "/" << activity_id
                  << " has begun: '" << title << "' (total=" << total_steps
                  << ")" << std::endl;
    }

    void activity_finished(
      const activity_progress_id_t parent_id,
      const activity_progress_id_t activity_id,
      const string_view title,
      const span_size_t total_steps) noexcept final {
        std::cout << "activity " << parent_id << "/" << activity_id
                  << " has has ended: '" << title << "' (total=" << total_steps
                  << ")" << std::endl;
    }

    void activity_updated(
      const activity_progress_id_t parent_id,
      const activity_progress_id_t activity_id,
      const span_size_t current,
      const span_size_t total) noexcept final {
        std::cout << "activity " << parent_id << "/" << activity_id
                  << " was updated: " << (100 * current / total) << std::endl;
    }
};
//------------------------------------------------------------------------------
auto main(main_ctx& ctx) -> int {
    example_observer observer;
    register_progress_observer(ctx, observer);

    const auto callback = [&]() {
        ctx.log().info("Progress callback called");
        return true;
    };
    set_progress_update_callback(
      ctx, {construct_from, callback}, std::chrono::milliseconds{100});

    const span_size_t m = 100;
    const span_size_t n = 1000;

    const auto main_act = ctx.progress().activity("Main activity", m);
    for(const auto i : integer_range(m)) {
        const auto sub_act = main_act.activity("Sub activity", n);
        for(const auto j : integer_range(n)) {
            sub_act.update_progress(j);
        }
        main_act.update_progress(i);
    }

    return 0;
}
//------------------------------------------------------------------------------
} // namespace eagine
