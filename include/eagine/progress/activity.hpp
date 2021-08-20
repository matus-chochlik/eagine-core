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

    ~activity_progress() noexcept {
        if(_backend) {
            _backend->finish_activity(_activity_id);
        }
    }

    /// @brief Creates a sub-activity progress tracker with a title.
    auto activity(
      const string_view title,
      const span_size_t total_steps = 0) noexcept -> activity_progress {
        return {*this, title, total_steps};
    }

    /// @brief Returns a pointer to the backend of this activity object.
    auto backend() const noexcept -> const auto& {
        return _backend;
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

protected:
    activity_progress(std::shared_ptr<progress_tracker_backend> backend) noexcept
      : _backend{std::move(backend)} {}

private:
    std::shared_ptr<progress_tracker_backend> _backend;
    const activity_progress_id_t _activity_id{0U};
};
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_PROGRESS_ACTIVITY_HPP
