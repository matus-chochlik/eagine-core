/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_PROGRESS_DEFAULT_BACKEND_HPP
#define EAGINE_PROGRESS_DEFAULT_BACKEND_HPP

#include "../maybe_unused.hpp"
#include "backend.hpp"

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Default implementation of activity progress backend implementations.
/// @ingroup progress
class default_progress_tracker_backend : public progress_tracker_backend {
public:
    default_progress_tracker_backend(main_ctx_getters&) {}

    auto begin_activity(
      const activity_progress_id_t parent_id,
      const string_view title,
      span_size_t total_steps) -> activity_progress_id_t final {
        // TODO
        EAGINE_MAYBE_UNUSED(parent_id);
        EAGINE_MAYBE_UNUSED(title);
        EAGINE_MAYBE_UNUSED(total_steps);
        return 1;
    }

    void update_progress(
      const activity_progress_id_t activity_id,
      span_size_t current) noexcept final {
        // TODO
        EAGINE_MAYBE_UNUSED(activity_id);
        EAGINE_MAYBE_UNUSED(current);

        const auto now = _clock.now();
        if(_last_call < now) {
            _last_call = now + _min_interval;
            if(_callback) {
                _callback();
            }
        }
    }

    void finish_activity(
      const activity_progress_id_t activity_id) noexcept final {
        // TODO
        EAGINE_MAYBE_UNUSED(activity_id);
    }

    void set_update_callback(
      const callable_ref<void()> callback,
      const std::chrono::milliseconds min_interval) final {
        _callback = callback;
        _min_interval = min_interval;
    }

private:
    callable_ref<void()> _callback{};
    std::chrono::milliseconds _min_interval{500};
    std::chrono::steady_clock::time_point _last_call{};
    std::chrono::steady_clock _clock;
};
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_PROGRESS_DEFAULT_BACKEND_HPP
