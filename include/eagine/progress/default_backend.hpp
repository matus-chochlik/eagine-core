/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_PROGRESS_DEFAULT_BACKEND_HPP
#define EAGINE_PROGRESS_DEFAULT_BACKEND_HPP

#include "../logging/logger.hpp"
#include "../maybe_unused.hpp"
#include "backend.hpp"
#include <atomic>
#include <map>
#include <mutex>

namespace eagine {
//------------------------------------------------------------------------------
struct default_progress_info {
    std::string title;
    span_size_t total_steps{0};
    span_size_t prev_steps{0};
    span_size_t curr_steps{0};
    span_size_t increment{0};
    activity_progress_id_t parent_id{0};
    log_event_severity log_level{log_event_severity::info};
};
//------------------------------------------------------------------------------
/// @brief Default implementation of activity progress backend implementations.
/// @ingroup progress
class default_progress_tracker_backend : public progress_tracker_backend {
public:
    default_progress_tracker_backend(main_ctx_getters& ctx)
      : _log{EAGINE_ID(Progress), ctx.log()} {}

    auto begin_activity(
      const activity_progress_id_t parent_id,
      const string_view title,
      span_size_t total_steps) -> activity_progress_id_t final {
        std::lock_guard<std::mutex> lock{_mutex};
        while(true) {
            if(++_id_sequence != 0) {
                if(_activities.find(_id_sequence) == _activities.end()) {
                    break;
                }
            }
        }
        auto& info = _activities[_id_sequence];
        info.title = to_string(title);
        info.total_steps = total_steps;
        info.increment = span_size_t(float(total_steps) * 0.001F);
        info.parent_id = parent_id;
        return reinterpret_cast<activity_progress_id_t>(&info);
    }

    auto update_progress(
      const activity_progress_id_t activity_id,
      span_size_t current) noexcept -> bool final {
        const auto now = _clock.now();
        if(_last_call < now) {
            _last_call = now + _min_interval;
            if(_callback) {
                _keep_going = _callback() && _keep_going;
            }
        }

        if(activity_id) {
            /// NOLINTNEXTLINE(performance-no-int-to-ptr)
            auto& info = *reinterpret_cast<default_progress_info*>(activity_id);
            info.curr_steps = current;
            if(info.curr_steps - info.prev_steps >= info.increment) {
                info.prev_steps = info.curr_steps;
                _do_log(info);
            }
        }
        return _keep_going;
    }

    void finish_activity(
      const activity_progress_id_t activity_id) noexcept final {
        if(activity_id) {
            /// NOLINTNEXTLINE(performance-no-int-to-ptr)
            _do_log(*reinterpret_cast<default_progress_info*>(activity_id));
            std::lock_guard<std::mutex> lock{_mutex};
            _activities.erase(activity_id);
        }
    }

    void set_update_callback(
      const callable_ref<bool()> callback,
      const std::chrono::milliseconds min_interval) final {
        _callback = callback;
        _min_interval = min_interval;
    }

private:
    void _do_log(const default_progress_info& info) const {
        _log.log(info.log_level, info.title)
          .arg(EAGINE_ID(current), info.curr_steps)
          .arg(EAGINE_ID(total), info.total_steps)
          .arg(
            EAGINE_ID(progress),
            EAGINE_ID(Progress),
            0.F,
            float(info.curr_steps),
            float(info.total_steps));
    }

    logger _log;
    callable_ref<bool()> _callback{};
    std::chrono::milliseconds _min_interval{500};
    std::chrono::steady_clock::time_point _last_call{};
    std::chrono::steady_clock _clock;
    activity_progress_id_t _id_sequence{0};
    std::map<activity_progress_id_t, default_progress_info> _activities;
    std::mutex _mutex;
    std::atomic<bool> _keep_going{true};
};
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_PROGRESS_DEFAULT_BACKEND_HPP
