/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.utility:timeout;

import std;
import eagine.core.types;
import eagine.core.debug;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Class that measures elapsed time since instance construction.
/// @ingroup time_utils
/// @see timeout
/// @see referencing_timeout
export class time_measure {
    using _clock = std::chrono::steady_clock;

public:
    /// @brief Default constructor. Saves the current time.
    time_measure() noexcept
      : _start{_clock::now()} {}

    /// @brief Returns the time since construction using clock's representation.
    [[nodiscard]] auto elapsed_time() const noexcept {
        return _clock::now() - _start;
    }

    /// @brief Returns the time since construction in seconds.
    [[nodiscard]] auto seconds() const noexcept {
        return std::chrono::duration_cast<std::chrono::duration<float>>(
          elapsed_time());
    }

private:
    _clock::time_point _start{};
};
//------------------------------------------------------------------------------
/// @brief Adjusts time duration in cases program execution is slowed down.
/// @ingroup time_utils
export template <typename R, typename P>
[[nodiscard]] constexpr auto adjusted_duration(
  std::chrono::duration<R, P> d,
  memory_access_rate mem_access = memory_access_rate::medium) noexcept {
    return d * R(temporal_slowdown_factor(mem_access));
}
//------------------------------------------------------------------------------
/// @brief Class representing a timeout since construction or reset.
/// @ingroup time_utils
/// @see time_measure
/// @see referencing_timeout
/// @see resetting_timeout
/// @see backing_off_timeout
export class timeout {
    using _clock = std::chrono::steady_clock;

public:
    /// @brief Alias for the duration type used by this timeout.
    using duration_type = _clock::duration;

    /// @brief Default construction. Saves current time. Immediately expires.
    /// @post is_expired()
    timeout() noexcept
      : _deadline{_clock::now()} {}

    /// @brief Construction from the default and initial timeout duration.
    /// @see reset
    /// @see period
    timeout(
      const _clock::duration duration,
      const _clock::duration initial) noexcept
      : _period{duration}
      , _deadline{_clock::now() + initial} {}

    /// @brief Construction from the default and zero initial timeout duration.
    /// @see reset
    /// @see period
    timeout(const _clock::duration duration, const nothing_t) noexcept
      : timeout{duration, _clock::duration::zero()} {}

    /// @brief Construction from the default duration.
    /// @see reset
    /// @see period
    timeout(const _clock::duration duration) noexcept
      : timeout{duration, duration} {}

    /// @brief Construction from the default duration.
    /// @see reset
    /// @see period
    template <typename R, typename P>
    timeout(const std::chrono::duration<R, P> duration) noexcept
      : timeout{std::chrono::duration_cast<_clock::duration>(duration)} {}

    /// @brief Sets new duration that will apply after next reset.
    /// @see reset
    template <typename Rd, typename Pd>
    auto set_duration(const std::chrono::duration<Rd, Pd> duration) noexcept
      -> auto& {
        _period = std::chrono::duration_cast<_clock::duration>(duration);
        return *this;
    }

    /// @brief Resets the timeout using the previously specified duration.
    /// @brief set_duration
    auto reset() noexcept -> auto& {
        _deadline = std::chrono::steady_clock::now() + _period;
        return *this;
    }

    /// @brief Resets the timeout using the specified duration values.
    /// @brief set_duration
    template <typename Rd, typename Pd, typename Ri, typename Pi>
    auto reset(
      const std::chrono::duration<Rd, Pd> duration,
      const std::chrono::duration<Ri, Pi> initial) noexcept -> auto& {
        _period = std::chrono::duration_cast<_clock::duration>(duration);
        _deadline = std::chrono::steady_clock::now() +
                    std::chrono::duration_cast<_clock::duration>(initial);
        return *this;
    }

    /// @brief Resets the timeout using the specified duration values.
    /// @brief set_duration
    /// @post is_expired()
    template <typename R, typename P>
    auto reset(
      const std::chrono::duration<R, P> duration,
      const nothing_t) noexcept -> auto& {
        return reset(duration, _clock::duration::zero());
    }

    /// @brief Resets the timeout using the specified duration value.
    /// @brief set_duration
    /// @see is_expired
    template <typename R, typename P>
    auto reset(const std::chrono::duration<R, P> duration) noexcept -> auto& {
        return reset(duration, duration);
    }

    /// @brief Returns the time since reset as clocks duration type.
    [[nodiscard]] auto elapsed_time() const noexcept -> duration_type {
        return _clock::now() - _deadline + _period;
    }

    /// @brief Indicates if the timeout is expired.
    /// @see reset
    /// @see period
    [[nodiscard]] auto is_expired() const noexcept -> bool {
        return _clock::now() >= _deadline;
    }

    /// @brief Indicates if the timeout is expired.
    /// @see is_expired
    [[nodiscard]] explicit operator bool() const noexcept {
        return is_expired();
    }

    /// @brief Returns the timeout period.
    /// @see reset
    /// @see is_expired
    [[nodiscard]] auto period() const noexcept -> const auto& {
        return _period;
    }

private:
    _clock::duration _period{};
    _clock::time_point _deadline{};
};
//------------------------------------------------------------------------------
/// @brief Class representing a timeout since construction or reset.
/// @ingroup time_utils
/// @see time_measure
/// @see timeout
/// @see resetting_timeout
export class referencing_timeout {
    using _clock = std::chrono::steady_clock;

public:
    /// @brief Alias for the duration type used by this timeout.
    using duration_type = _clock::duration;

    /// @brief Constructor initializing the reference to the timeout interval.
    referencing_timeout(const duration_type& interval) noexcept
      : _period{interval}
      , _prev_reset{_clock::now()} {}

    /// @brief Resets the timeout using the referenced specified duration.
    auto reset() noexcept -> auto& {
        _prev_reset = std::chrono::steady_clock::now();
        return *this;
    }

    /// @brief Returns the time since reset as clocks duration type.
    [[nodiscard]] auto elapsed_time() const noexcept -> duration_type {
        return _clock::now() - _prev_reset;
    }

    /// @brief Indicates if the timeout is expired.
    /// @see reset
    /// @see period
    [[nodiscard]] auto is_expired() const noexcept -> bool {
        return _clock::now() >= _prev_reset + _period;
    }

    /// @brief Indicates if the timeout is expired.
    /// @see is_expired
    [[nodiscard]] explicit operator bool() const noexcept {
        return is_expired();
    }

    /// @brief Returns the timeout period.
    /// @see reset
    /// @see is_expired
    [[nodiscard]] auto period() const noexcept -> const auto& {
        return _period;
    }

private:
    const _clock::duration& _period;
    _clock::time_point _prev_reset{};
};
//------------------------------------------------------------------------------
/// @brief Specialization of timeout that resets when tested for expiration.
/// @ingroup time_utils
/// @see timeout
/// @see referencing_timeout
/// @see backing_off_timeout
/// @see time_measure
export class resetting_timeout : public timeout {
public:
    using timeout::timeout;

    /// @brief Indicates if expired and if it is resets the timeout.
    [[nodiscard]] explicit operator bool() noexcept {
        const auto result = is_expired();
        if(result) {
            reset();
        }
        return result;
    }
};
//------------------------------------------------------------------------------
/// @brief Timeout that can be doubled (extended) or reset back to minimum duration.
/// @ingroup time_utils
/// @see timeout
/// @see resetting_timeout
export class backing_off_timeout {
    using _clock = std::chrono::steady_clock;

public:
    /// @brief Alias for the duration type used by this timeout.
    using duration_type = _clock::duration;

    /// @brief Construction from min and max period.
    template <typename Rmin, typename Pmin, typename Rmax, typename Pmax>
    backing_off_timeout(
      std::chrono::duration<Rmin, Pmin> min,
      std::chrono::duration<Rmax, Pmax> max) noexcept
      : _min_period{std::chrono::duration_cast<_clock::duration>(min)}
      , _max_period{std::chrono::duration_cast<_clock::duration>(max)}
      , _cur_period{_min_period}
      , _deadline{_clock::now() + _cur_period} {}

    /// @brief Construction from min and max period.
    template <typename Rmin, typename Pmin, typename Rmax, typename Pmax>
    backing_off_timeout(
      std::chrono::duration<Rmin, Pmin> min,
      std::chrono::duration<Rmax, Pmax> max,
      nothing_t) noexcept
      : _min_period{std::chrono::duration_cast<_clock::duration>(min)}
      , _max_period{std::chrono::duration_cast<_clock::duration>(max)}
      , _cur_period{_min_period}
      , _deadline{_clock::now()} {}

    /// @brief Indicates if the timeout is expired.
    /// @see reset
    /// @see rewind
    /// @see extend
    auto is_expired() const noexcept -> bool {
        return _clock::now() >= _deadline;
    }

    /// @brief Returns the timeout period.
    /// @see reset
    /// @see is_expired
    auto period() const noexcept -> duration_type {
        return _cur_period;
    }

    /// @brief Sets a new deadline to period, starting from now.
    /// @see period
    /// @see rewind
    auto reset() noexcept -> backing_off_timeout& {
        _deadline = _clock::now() + _cur_period;
        return *this;
    }

    /// @brief Resets the period to minimum and sets a new deadline.
    /// @see reset
    /// @see extend
    auto rewind() noexcept -> backing_off_timeout& {
        _cur_period = _min_period;
        return reset();
    }

    /// @brief Extends the period to double of the current or max and set new deadline.
    /// @see reset
    /// @see rewind
    /// @see period
    auto extend() noexcept -> backing_off_timeout& {
        _cur_period =
          std::min(_cur_period * duration_type::rep(2), _max_period);
        return reset();
    }

private:
    duration_type _min_period;
    duration_type _max_period;
    duration_type _cur_period;
    _clock::time_point _deadline{};
};
//------------------------------------------------------------------------------
} // namespace eagine

