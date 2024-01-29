/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
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
export class action_scheduler : std::chrono::steady_clock {
public:
    /// @brief The smallest time granularity distinguished by the scheduler.
    using duration_type = std::chrono::microseconds;

    /// @brief Schedule a repeated action identified by id with specified interval.
    /// @see suspend
    /// @see resume
    /// @see remove
    /// @see has_scheduled
    auto schedule_repeated(
      const identifier id,
      const duration_type interval,
      std::function<bool()> action) -> action_scheduler&;

    /// @brief Indicates if action with the specified unique id is scheduled.
    /// @see schedule_repeated
    /// @see remove
    /// @see suspend
    auto has_scheduled(const identifier id) const noexcept -> bool;

    /// @brief Suspends the invocation of action identified by id without removing.
    /// @see is_suspended
    /// @see resume
    /// @see remove
    auto suspend(const identifier id) noexcept -> bool;

    /// @brief Indicates if the action identified by id is suspended.
    /// @see suspend
    /// @see resume
    auto is_suspend(const identifier id) noexcept -> bool;

    /// @brief Removes the invocation of a previously suspended action identified by id.
    /// @see is_suspended
    /// @see suspend
    /// @see remove
    auto resume(const identifier id) noexcept -> bool;

    /// @brief Removed the action with the specified unique id.
    /// @see schedule_repeated
    /// @see has_scheduled
    auto remove(const identifier id) -> action_scheduler&;

    /// @brief Calls all the scheduled actions that are due.
    /// @see schedule_repeated
    auto update() noexcept -> action_scheduler&;

private:
    struct _scheduled {
        std::chrono::steady_clock::time_point next;
        duration_type interval;
        std::function<bool()> action;
        std::uint32_t fail_counter{0U};
        bool is_suspended{false};

        auto should_invoke() noexcept -> bool;
        void invoke() noexcept;
        void update(const std::chrono::steady_clock::time_point) noexcept;
    };
    flat_map<identifier, _scheduled> _repeated;
};
//------------------------------------------------------------------------------
} // namespace eagine
