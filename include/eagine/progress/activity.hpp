/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_PROGRESS_ACTIVITY_HPP
#define EAGINE_PROGRESS_ACTIVITY_HPP

#include "backend.hpp"
#include <memory>

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Class tracking the progress of a long-running activity.
/// @ingroup progress
class activity_progress {
public:
    activity_progress(
      const activity_progress& parent,
      const string_view title,
      const span_size_t total_steps = 0) noexcept
      : _backend{parent._backend}
      , _activity_id{
          _backend
            ? _backend->begin_activity(parent._activity_id, title, total_steps)
            : 0U} {}

    activity_progress(activity_progress&& temp) noexcept
      : _backend{std::move(temp._backend)}
      , _activity_id{temp._activity_id} {}
    activity_progress(const activity_progress&) = delete;
    auto operator=(activity_progress&&) = delete;
    auto operator=(const activity_progress&) = delete;

    /// @brief Marks the activity as finished.
    /// @see finish
    ~activity_progress() noexcept {
        finish();
    }

    /// @brief Creates a sub-activity progress tracker with a title.
    auto activity(const string_view title, const span_size_t total_steps = 0)
      const noexcept -> activity_progress {
        return {*this, title, total_steps};
    }

    /// @brief Returns the unique id of this logger instance.
    auto activity_id() const noexcept -> activity_progress_id_t {
        return _activity_id;
    }

    auto configure(application_config& config) const -> bool {
        if(auto pbe{backend()}) {
            extract(pbe).configure(config);
        }
        return false;
    }

    /// @brief Updates the activity progress to the specified current value.
    /// @see finish
    /// @see advance_progress
    ///
    /// The return value indicates if the activity was canceled, true values
    /// means that the activity should continue, false means that the activity
    /// was canceled and should discontinue.
    auto update_progress(span_size_t current) const noexcept -> bool {
        if(auto pbe{backend()}) {
            return extract(pbe).update_progress(_activity_id, current);
        }
        return true;
    }

    /// @brief Advances the activity progress by the specified increment.
    /// @see finish
    /// @see update_progress
    ///
    /// The return value indicates if the activity was canceled, true values
    /// means that the activity should continue, false means that the activity
    /// was canceled and should discontinue.
    auto advance_progress(span_size_t increment = 1) const noexcept -> bool {
        if(auto pbe{backend()}) {
            return extract(pbe).advance_progress(_activity_id, increment);
        }
        return true;
    }

    /// @brief Explicitly marks the activity as finished.
    /// @see advance_progress
    /// @see update_progress
    void finish() noexcept {
        if(_backend) {
            _backend->finish_activity(_activity_id);
            _backend.reset();
        }
    }

protected:
    activity_progress(std::shared_ptr<progress_tracker_backend> backend) noexcept
      : _backend{std::move(backend)} {}

    auto backend() const noexcept
      -> const std::shared_ptr<progress_tracker_backend>& {
        return _backend;
    }

private:
    std::shared_ptr<progress_tracker_backend> _backend;
    const activity_progress_id_t _activity_id{0U};
};
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_PROGRESS_ACTIVITY_HPP
