/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_ANIMATED_VALUE_HPP
#define EAGINE_ANIMATED_VALUE_HPP

#include "math/functions.hpp"
#include "valid_if/positive.hpp"
#include <type_traits>

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Class representing a value animated between two points, with easing.
template <typename T, typename F, typename S = float>
class animated_value {
public:
    /// @brief Default constructor.
    animated_value() = default;

    /// @brief Initializing to the specified value.
    animated_value(const T& initial)
      : _curr{initial}
      , _next{initial} {}

    /// @brief Sets the next point to animate into, duration and sigmoid slope.
    auto set(const T value, const valid_if_positive<F> duration, const S slope)
      -> auto& {
        _curr = _next;
        _next = value;
        _duration = extract(duration);
        _phase = F(0);
        _slope = slope;
        return *this;
    }

    /// @brief Sets the next point to animate into and the duration of the transition.
    auto set(const T value, const valid_if_positive<F> duration) -> auto& {
        return set(value, duration, 2);
    }

    /// @brief Gets the current value between the current and the next point.
    auto get() const -> T {
        return math::blend(
          _curr,
          _next,
          math::sigmoid01(math::clamp(_phase / _duration, 0, 1), _slope));
    }

    /// @brief Updates the transition factor by the specified amount.
    auto update(const F deltaT) -> auto& {
        _phase += deltaT;
        return *this;
    }

    /// @brief Indicates if the current transition factor is higher than current duration.
    auto is_done() const noexcept -> bool {
        return _phase >= _duration;
    }

private:
    T _curr{};
    T _next{};
    F _duration{1};
    F _phase{_duration};
    S _slope{2};
};
//------------------------------------------------------------------------------
} // namespace eagine

#endif
