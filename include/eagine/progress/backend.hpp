/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_PROGRESS_BACKEND_HPP
#define EAGINE_PROGRESS_BACKEND_HPP

#include "../interface.hpp"
#include "../string_span.hpp"
#include "../types.hpp"
#include "fwd.hpp"

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

    /// @brief Specifies the current number of steps in the done in the activity.
    virtual void update_progress(
      const activity_progress_id_t activity_id,
      span_size_t current) = 0;

    /// @brief Indicates that an activity has finished.
    virtual void finish_activity(
      const activity_progress_id_t activity_id) noexcept = 0;
};
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_PROGRESS_BACKEND_HPP
