/// @example eagine/progress.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import std;

namespace eagine {
//------------------------------------------------------------------------------
class example_observer final
  : public main_ctx_object
  , public progress_observer {
public:
    example_observer(main_ctx_parent parent) noexcept
      : main_ctx_object{"Observer", parent} {}

    void activity_begun(
      const activity_progress_id_t parent_id,
      const activity_progress_id_t activity_id,
      const string_view title,
      const span_size_t total_steps) noexcept final {
        cio_print(
          "activity ${parent}/${activity} has begun: '${title}' "
          "(total=${total})")
          .arg("parent", parent_id)
          .arg("activity", activity_id)
          .arg("total", total_steps)
          .arg("title", title);
    }

    void activity_finished(
      const activity_progress_id_t parent_id,
      const activity_progress_id_t activity_id,
      const string_view title,
      const span_size_t total_steps) noexcept final {
        cio_print(
          "activity ${parent}/${activity} has ended: '${title}' "
          "(total=${total})")
          .arg("parent", parent_id)
          .arg("activity", activity_id)
          .arg("total", total_steps)
          .arg("title", title);
    }

    void activity_updated(
      const activity_progress_id_t parent_id,
      const activity_progress_id_t activity_id,
      const span_size_t current,
      const span_size_t total) noexcept final {
        cio_print(
          "activity ${parent}/${activity} was updated: "
          "${current}")
          .arg("parent", parent_id)
          .arg("activity", activity_id)
          .arg("current", current, total);
    }
};
//------------------------------------------------------------------------------
auto main(main_ctx& ctx) -> int {
    example_observer observer{ctx};
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

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

