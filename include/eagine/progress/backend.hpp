/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_PROGRESS_BACKEND_HPP
#define EAGINE_PROGRESS_BACKEND_HPP

#include "../callable_ref.hpp"
#include "../interface.hpp"
#include "../string_span.hpp"
#include "../types.hpp"
#include "fwd.hpp"
#include <chrono>

namespace eagine {
class application_config;
//------------------------------------------------------------------------------
/// @brief Interface for activity progress backend implementations.
/// @ingroup progress
struct progress_tracker_backend : interface<progress_tracker_backend> {
    virtual auto configure(application_config&) -> bool {
        return false;
    }

    /// @brief Requests the id for a new activity progress.
    virtual auto begin_activity(
      const activity_progress_id_t parent_id,
      const string_view title,
      span_size_t total_steps) -> activity_progress_id_t = 0;

    /// @brief Specifies the current number of steps done in the activity.
    ///
    /// The return value indicates if the activity was canceled, true values
    /// means that the activity should continue, false means that the activity
    /// was canceled and should discontinue.
    virtual auto update_progress(
      const activity_progress_id_t activity_id,
      span_size_t current) noexcept -> bool = 0;

    /// @brief Advances the current number of steps done in the activity.
    ///
    /// The return value indicates if the activity was canceled, true values
    /// means that the activity should continue, false means that the activity
    /// was canceled and should discontinue.
    virtual auto advance_progress(
      const activity_progress_id_t activity_id,
      span_size_t increment) noexcept -> bool = 0;

    /// @brief Indicates that an activity has finished.
    virtual void finish_activity(
      const activity_progress_id_t activity_id) noexcept = 0;

    /// @brief Assigns a function to be called on progress update.
    virtual void set_update_callback(
      const callable_ref<bool()>,
      const std::chrono::milliseconds min_interval) = 0;
};
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_PROGRESS_BACKEND_HPP
