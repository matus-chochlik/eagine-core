/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.utility:scheduler;

import std;
import eagine.core.types;
import eagine.core.container;
import eagine.core.identifier;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Class that allows to schedule invocation of actions in periodic intervals.
/// @ingroup time_utils
export class scheduler {
public:
    /// @brief The smallest time granularity distinguished by the scheduler.
    using duration_type = std::chrono::microseconds;

    /// @brief Schedule a repeated action identified by id with specified interval.
    /// @see remove
    /// @see has_scheduled
    auto schedule_repeated(
      const identifier id,
      const duration_type interval,
      std::function<void()> action) -> scheduler&;

    /// @brief Indicates if action with the specified unique id is scheduled.
    /// @see schedule_repeated
    /// @see remove
    auto has_scheduled(const identifier id) const noexcept -> bool {
        return _repeated.contains(id);
    }

    /// @brief Removed the action with the specified unique id.
    /// @see schedule_repeated
    /// @see has_scheduled
    auto remove(const identifier id) -> scheduler&;

    /// @brief Calls all the scheduled actions that are due.
    /// @see schedule_repeated
    auto update() noexcept -> scheduler&;

private:
    std::chrono::steady_clock _clock;

    struct _scheduled {
        std::chrono::steady_clock::time_point next;
        duration_type interval;
        std::function<void()> action;
    };
    flat_map<identifier, _scheduled> _repeated;
};
//------------------------------------------------------------------------------
} // namespace eagine
