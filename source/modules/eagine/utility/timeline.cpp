/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.utility:timeline;

import eagine.core.types;
import <cstdint>;
import <chrono>;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Class representing a stateful timeline, with a chrono-like API.
/// @ingroup time_utils
/// @see timeout
export class timeline {
public:
    /// @brief The representation type.
    using rep = std::int64_t;

    /// @brief The period type.
    using period = std::nano;

    /// @brief The duration type.
    using duration = std::chrono::duration<rep, period>;

    /// @brief The time-point type.
    using time_point = std::chrono::time_point<timeline>;

    /// @brief Indicates that this clock is not steady.
    static constexpr const bool is_steady = false;

    /// @brief Default constructor.
    constexpr timeline() noexcept = default;

    /// @brief Constructor from an time offset since the start of the clock's epoch.
    template <typename R, typename P>
    constexpr timeline(const std::chrono::duration<R, P> offset) noexcept {
        _now += offset;
    }

    /// @brief Returns the current time on the clock.
    [[nodiscard]] constexpr auto now() const noexcept -> time_point {
        return _now;
    }

    /// @brief Advance the time on the timeline by the specified period.
    template <typename R, typename P>
    constexpr auto advance(const std::chrono::duration<R, P> period) noexcept
      -> timeline& {
        _now += period;
        return *this;
    }

private:
    time_point _now{};
};
//------------------------------------------------------------------------------
} // namespace eagine
