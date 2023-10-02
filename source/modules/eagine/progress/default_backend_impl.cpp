/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.progress;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.identifier;
import eagine.core.container;
import eagine.core.utility;
import eagine.core.runtime;
import eagine.core.logging;
import :backend;

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
    default_progress_tracker_backend(logger& parent)
      : _log{"Progress", parent} {}

    auto register_observer(progress_observer& observer) noexcept -> bool final {
        const std::lock_guard<std::mutex> lock{_mutex};
        if(not _observer) {
            _observer = {observer};
            return true;
        }
        return false;
    }

    void unregister_observer(progress_observer& observer) noexcept final {
        const std::lock_guard<std::mutex> lock{_mutex};
        if(_observer.refers_to(observer)) {
            _observer = {};
        }
    }

    auto begin_activity(
      const activity_progress_id_t parent_id,
      const string_view title,
      span_size_t total_steps) -> activity_progress_id_t final {
        auto id_and_info =
          [&]() -> std::tuple<activity_progress_id_t, default_progress_info&> {
            const std::lock_guard<std::mutex> lock{_mutex};
            while(true) {
                if(++_id_sequence != 0) {
                    if(not _activities.contains(_id_sequence)) {
                        break;
                    }
                }
            }
            auto& result = _activities[_id_sequence];
            const auto activity_id = _encode(result);
            _index[activity_id] = _id_sequence;
            if(_observer) {
                _observer->activity_begun(
                  parent_id, activity_id, title, total_steps);
            }
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
            _update(activity_id, _decode(activity_id), current);
        }
        return _keep_going;
    }

    auto advance_progress(
      const activity_progress_id_t activity_id,
      span_size_t increment) noexcept -> bool final {
        _call_back();
        if(activity_id) {
            auto& info = _decode(activity_id);
            _update(activity_id, info, info.curr_steps + increment);
        }
        return _keep_going;
    }

    void finish_activity(
      const activity_progress_id_t activity_id) noexcept final {
        if(activity_id) {
            auto& info = _decode(activity_id);
            info.curr_steps = info.total_steps;
            _do_log(info);
            const std::lock_guard<std::mutex> lock{_mutex};
            const auto ipos = _index.find(activity_id);
            if(ipos != _index.end()) {
                if(_observer) {
                    const auto apos = _activities.find(ipos->second);
                    if(apos != _activities.end()) {
                        auto& activity = apos->second;
                        _observer->activity_finished(
                          activity.parent_id,
                          activity_id,
                          activity.title,
                          activity.total_steps);
                    }
                }
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

    void reset_update_callback() noexcept final {
        _callback = {};
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
                _keep_going = _callback() and _keep_going;
            }
        }
    }

    void _update(
      const activity_progress_id_t activity_id,
      default_progress_info& info,
      span_size_t current) noexcept {
        info.curr_steps = current;
        if(info.curr_steps - info.prev_steps >= info.increment) {
            info.prev_steps = info.curr_steps;
            _do_log(info);
        }
        const std::lock_guard<std::mutex> lock{_mutex};
        if(_observer) {
            _observer->activity_updated(
              info.parent_id, activity_id, info.curr_steps, info.total_steps);
        }
    }

    auto _get_tag(const default_progress_info& info) const noexcept
      -> identifier {
        return info.parent_id ? identifier{"Progress"}
                              : identifier{"MainPrgrss"};
    }

    void _do_log(const default_progress_info& info) const noexcept {
        _log.log(info.log_level, info.title)
          .arg("current", info.curr_steps)
          .arg("total", info.total_steps)
          .arg(
            "progress",
            _get_tag(info),
            0.F,
            float(info.curr_steps),
            float(info.total_steps));
    }

    std::mutex _mutex;
    logger _log;
    callable_ref<bool() noexcept> _callback{};
    optional_reference<progress_observer> _observer{};
    std::chrono::milliseconds _min_interval{500};
    std::chrono::steady_clock::time_point _last_call{};
    std::chrono::steady_clock _clock;
    activity_progress_id_t _id_sequence{0};
    flat_map<activity_progress_id_t, activity_progress_id_t> _index;
    std::map<activity_progress_id_t, default_progress_info> _activities;
    std::atomic<bool> _keep_going{true};
};
//------------------------------------------------------------------------------
auto make_default_progress_tracker_backend(logger& parent)
  -> unique_holder<progress_tracker_backend> {
    return {hold<default_progress_tracker_backend>, parent};
}
//------------------------------------------------------------------------------
} // namespace eagine
