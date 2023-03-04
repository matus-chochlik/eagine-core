/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.progress:backend;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.utility;
import eagine.core.runtime;

import std;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Activity progress tracker instance id type.
/// @ingroup progress
export using activity_progress_id_t = std::uintptr_t;
//------------------------------------------------------------------------------
/// @brief Interface for classes observing activity progress.
/// @ingroup progress
export struct progress_observer : interface<progress_observer> {
    /// @brief Called when a new activity has begun.
    virtual void activity_begun(
      const activity_progress_id_t parent_id,
      const activity_progress_id_t activity_id,
      const string_view title,
      const span_size_t total_steps) noexcept = 0;

    /// @brief Called when an existing activity has finished.
    virtual void activity_finished(
      const activity_progress_id_t parent_id,
      const activity_progress_id_t activity_id,
      const string_view title,
      span_size_t total_steps) noexcept = 0;

    /// @brief Called when the activity progress has changed.
    virtual void activity_updated(
      const activity_progress_id_t parent_id,
      const activity_progress_id_t activity_id,
      const span_size_t current,
      const span_size_t total) noexcept = 0;
};
//------------------------------------------------------------------------------
/// @brief Interface for activity progress backend implementations.
/// @ingroup progress
struct progress_tracker_backend : interface<progress_tracker_backend> {
    virtual auto configure(basic_config&) -> bool {
        return false;
    }

    /// @brief Registers an observer object to be notified.
    /// @see unregister_observer
    virtual auto register_observer(progress_observer&) noexcept -> bool = 0;

    /// @brief Un-registers an observer object to be notified.
    /// @see register_observer
    virtual void unregister_observer(progress_observer&) noexcept = 0;

    /// @brief Requests the id for a new activity progress.
    /// @see finish_activity
    virtual auto begin_activity(
      const activity_progress_id_t parent_id,
      const string_view title,
      span_size_t total_steps) -> activity_progress_id_t = 0;

    /// @brief Specifies the current number of steps done in the activity.
    /// @see advance_progress
    ///
    /// The return value indicates if the activity was canceled, true values
    /// means that the activity should continue, false means that the activity
    /// was canceled and should discontinue.
    virtual auto update_progress(
      const activity_progress_id_t activity_id,
      span_size_t current) noexcept -> bool = 0;

    /// @brief Advances the current number of steps done in the activity.
    /// @see update_progress
    ///
    /// The return value indicates if the activity was canceled, true values
    /// means that the activity should continue, false means that the activity
    /// was canceled and should discontinue.
    virtual auto advance_progress(
      const activity_progress_id_t activity_id,
      span_size_t increment) noexcept -> bool = 0;

    /// @brief Indicates that an activity has finished.
    /// @see begin_activity
    virtual void finish_activity(
      const activity_progress_id_t activity_id) noexcept = 0;

    /// @brief Assigns a function to be called on progress update.
    /// @see reset_update_callback
    virtual void set_update_callback(
      const callable_ref<bool() noexcept>,
      const std::chrono::milliseconds min_interval) = 0;

    /// @brief Resets the function called on progress update.
    /// @see set_update_callback
    virtual void reset_update_callback() noexcept = 0;
};
//------------------------------------------------------------------------------
} // namespace eagine
