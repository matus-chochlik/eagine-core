/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.progress:root_activity;

import eagine.core.runtime;
import eagine.core.utility;
import :backend;
import :activity;
import <chrono>;
import <memory>;

namespace eagine {
struct main_ctx_getters;
//------------------------------------------------------------------------------
export class root_activity : public activity_progress {
public:
    root_activity(main_ctx_getters& ctx) noexcept
      : activity_progress{_init_backend(ctx)} {}

    auto register_observer(progress_observer&) noexcept -> bool;
    void unregister_observer(progress_observer&) noexcept;

    void set_update_callback(
      const callable_ref<bool() noexcept> callback,
      const std::chrono::milliseconds min_interval) noexcept;

    void reset_update_callback() noexcept;

private:
    static auto _init_backend(main_ctx_getters&)
      -> std::unique_ptr<progress_tracker_backend>;
};
//------------------------------------------------------------------------------
} // namespace eagine
