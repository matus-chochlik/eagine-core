/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_PROGRESS_DEFAULT_BACKEND_HPP
#define EAGINE_PROGRESS_DEFAULT_BACKEND_HPP

#include "../flat_map.hpp"
#include "../logging/logger.hpp"
#include "../maybe_unused.hpp"
#include "backend.hpp"
#include <atomic>
#include <map>
#include <mutex>
#include <tuple>

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
        auto id_and_info =
          [&]() -> std::tuple<activity_progress_id_t, default_progress_info&> {
            const std::lock_guard<std::mutex> lock{_mutex};
            while(true) {
                if(++_id_sequence != 0) {
                    if(_activities.find(_id_sequence) == _activities.end()) {
                        break;
                    }
                }
            }
            auto& result = _activities[_id_sequence];
            const auto activity_id = _encode(result);
            _index[activity_id] = _id_sequence;
            return {activity_id, result};
        }();
        const auto activity_id = std::get<0>(id_and_info);
        auto& info = std::get<1>(id_and_info);
        info.title = to_string(title);
        info.total_steps = total_steps;
        info.increment = span_size_t(float(total_steps) * 0.001F);
        info.parent_id = parent_id;
        return activity_id;
    }

    auto update_progress(
      const activity_progress_id_t activity_id,
      span_size_t current) noexcept -> bool final {
        _call_back();
        if(activity_id) {
            _update(_decode(activity_id), current);
        }
        return _keep_going;
    }

    auto advance_progress(
      const activity_progress_id_t activity_id,
      span_size_t increment) noexcept -> bool final {
        _call_back();
        if(activity_id) {
            auto& info = _decode(activity_id);
            _update(info, info.curr_steps + increment);
        }
        return _keep_going;
    }

    void finish_activity(
      const activity_progress_id_t activity_id) noexcept final {
        if(activity_id) {
            _do_log(_decode(activity_id));
            const std::lock_guard<std::mutex> lock{_mutex};
            const auto ipos = _index.find(activity_id);
            if(ipos != _index.end()) {
                _activities.erase(ipos->second);
                _index.erase(ipos);
            }
        }
    }

    void set_update_callback(
      const callable_ref<bool() noexcept> callback,
      const std::chrono::milliseconds min_interval) final {
        _callback = callback;
        _min_interval = min_interval;
    }

private:
    static auto _encode(const default_progress_info& info) noexcept
      -> activity_progress_id_t {
        return reinterpret_cast<activity_progress_id_t>(&info);
    }

    static auto _decode(const activity_progress_id_t activity_id) noexcept
      -> default_progress_info& {
        /// NOLINTNEXTLINE(performance-no-int-to-ptr)
        return *reinterpret_cast<default_progress_info*>(activity_id);
    }

    void _call_back() noexcept {
        const auto now = _clock.now();
        if(_last_call < now) {
            _last_call = now + _min_interval;
            if(_callback) {
                _keep_going = _callback() && _keep_going;
            }
        }
    }

    void _update(default_progress_info& info, span_size_t current) {
        info.curr_steps = current;
        if(info.curr_steps - info.prev_steps >= info.increment) {
            info.prev_steps = info.curr_steps;
            _do_log(info);
        }
    }

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
    callable_ref<bool() noexcept> _callback{};
    std::chrono::milliseconds _min_interval{500};
    std::chrono::steady_clock::time_point _last_call{};
    std::chrono::steady_clock _clock;
    activity_progress_id_t _id_sequence{0};
    flat_map<activity_progress_id_t, activity_progress_id_t> _index;
    std::map<activity_progress_id_t, default_progress_info> _activities;
    std::mutex _mutex;
    std::atomic<bool> _keep_going{true};
};
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_PROGRESS_DEFAULT_BACKEND_HPP
