/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.types:tribool;

import std;
import :concepts;

namespace eagine {

enum class _tribool_value_t : unsigned char {
    _false = 0x00,
    _true = 0x01,
    _unknown = 0x02
};

/// @brief Type of the @c indeterminate constant assignable to @c tribool.
export struct indeterminate_t {};

/// @brief Constant representing unspecified @c tribool value.
export constexpr const indeterminate_t indeterminate = {};

export class tribool;

/// @brief Weak tri-state boolean value.
/// @see tribool
/// @see indeterminate
export class weakbool {
    using _value_t = _tribool_value_t;

public:
    /// @brief Returns true, if the stored value is not @c false.
    [[nodiscard]] constexpr explicit operator bool() const noexcept {
        return _value != _value_t::_false;
    }

    /// @brief Returns true, if the stored value is not @c true.
    [[nodiscard]] constexpr auto operator!() const noexcept -> bool {
        return _value != _value_t::_true;
    }

    ///@ brief Checks if the stored value is indeterminate.
    [[nodiscard]] constexpr auto is(const indeterminate_t) const noexcept
      -> bool {
        return _value == _value_t::_unknown;
    }

private:
    _value_t _value;

    friend class tribool;

    constexpr explicit weakbool(const _value_t value) noexcept
      : _value{value} {}
};

/// @brief Tri-state boolean value.
/// @see weakbool
/// @see indeterminate
export class tribool {
    using _value_t = _tribool_value_t;

public:
    using value_type = bool;

    /// @brief Default constructor.
    constexpr tribool() noexcept = default;

    /// @brief Constructions from boolean value.
    constexpr tribool(const bool value) noexcept
      : _value{value ? _value_t::_true : _value_t::_false} {}

    /// @brief Constructions from indeterminate value.
    constexpr tribool(const indeterminate_t) noexcept
      : _value{_value_t::_unknown} {}

    /// @brief Construction with separate true/false, known/unknown arguments.
    constexpr tribool(const bool value, const bool is_known) noexcept
      : _value{
          not is_known ? _value_t::_unknown
          : value      ? _value_t::_true
                       : _value_t::_false} {}

    /// @brief Construction from optional<bool>.
    constexpr tribool(const std::optional<bool>& opt) noexcept
      : _value{
          not opt ? _value_t::_unknown
          : *opt  ? _value_t::_true
                  : _value_t::_false} {}

    /// @brief Returns true, if the stored value is true.
    [[nodiscard]] constexpr explicit operator bool() const noexcept {
        return _value == _value_t::_true;
    }

    /// @brief Returns true, if the stored value is false.
    [[nodiscard]] constexpr auto operator!() const noexcept -> bool {
        return _value == _value_t::_false;
    }

    /// @brief Returns true, if the stored value is not indeterminate.
    [[nodiscard]] constexpr auto has_value() const noexcept -> bool {
        return _value != _value_t::_unknown;
    }

    /// @brief Resets the value to indeterminate.
    /// @post not has_value()
    constexpr auto reset() noexcept -> tribool& {
        _value = _value_t::_unknown;
        return *this;
    }

    /// @brief Returns the store value if not indeterminate.
    /// @pre has_value()
    [[nodiscard]] constexpr auto value() const noexcept -> bool {
        assert(has_value());
        return bool(*this);
    }

    /// @brief Returns the boolean value in not indeterminate, fallback otherwise.
    /// @see or_default
    [[nodiscard]] constexpr auto value_or(bool fallback) const noexcept
      -> bool {
        if(has_value()) {
            return bool(*this);
        }
        return fallback;
    }

    /// @brief Returns the boolean value if not indeterminate, true otherwise.
    /// @see value_or
    [[nodiscard]] constexpr auto or_true() const noexcept -> bool {
        return value_or(true);
    }

    /// @brief Returns the boolean value if not indeterminate, false otherwise.
    /// @see value_or
    [[nodiscard]] constexpr auto or_false() const noexcept -> bool {
        return value_or(false);
    }

    /// @brief Returns the boolean value if not indeterminate, false otherwise.
    /// @see value_or
    [[nodiscard]] constexpr auto or_default() const noexcept -> bool {
        return or_false();
    }

    /// @brief Invoke function on the stored value or return empty optional-like.
    /// @see transform
    /// @see or_else
    template <
      typename F,
      optional_like R = std::remove_cvref_t<std::invoke_result_t<F, bool>>>
    constexpr auto and_then(F&& function) const
      noexcept(noexcept(std::invoke(std::forward<F>(function), true))) -> R {
        if(has_value()) {
            return std::invoke(std::forward<F>(function), bool(*this));
        } else {
            if constexpr(std::same_as<R, tribool>) {
                return tribool{indeterminate};
            } else {
                return R{};
            }
        }
    }

    /// @brief Return the stored value or the result of function.
    /// @see transform
    /// @see and_then
    template <
      typename F,
      typename R = std::remove_cvref_t<std::invoke_result_t<F>>>
        requires(std::same_as<R, tribool> or std::convertible_to<R, tribool>)
    constexpr auto or_else(F&& function) const
      noexcept(noexcept(std::invoke_r<tribool>(std::forward<F>(function))))
        -> tribool {
        if(has_value()) {
            return *this;
        } else {
            return std::invoke_r<tribool>(std::forward<F>(function));
        }
    }

    /// @brief Invoke function on the stored value or return empty optional-like.
    /// @see and_then
    /// @see or_else
    template <
      typename F,
      typename R = std::remove_cvref_t<std::invoke_result_t<F, bool>>>
    [[nodiscard]] constexpr auto transform(F&& function) const noexcept(
      noexcept(std::invoke(std::forward<F>(function), true)) and
      std::is_nothrow_move_constructible_v<R>) {
        if constexpr(std::is_same_v<R, bool>) {
            if(has_value()) {
                return tribool{
                  std::invoke(std::forward<F>(function), bool(*this)), true};
            } else {
                return tribool{indeterminate};
            }
        } else {
            if(has_value()) {
                return std::optional<R>{
                  std::invoke(std::forward<F>(function), bool(*this))};
            } else {
                return std::optional<R>{};
            }
        }
    }

    /// @brief Returns the store value if not indeterminate.
    /// @pre has_value()
    [[nodiscard]] constexpr auto operator*() const noexcept -> bool {
        return value();
    }

    /// @brief Converts this value to @c weakbool.
    [[nodiscard]] constexpr auto operator~() const noexcept -> weakbool {
        return weakbool{_value};
    }

    /// @brief Returns true if the stored value is known and equal to @p value.
    [[nodiscard]] constexpr auto is(const bool value) const noexcept -> bool {
        return _value == (value ? _value_t::_true : _value_t::_false);
    }

    /// @brief Returns true if the stored value is indeterminate.
    [[nodiscard]] constexpr auto is(const indeterminate_t) const noexcept
      -> bool {
        return _value == _value_t::_unknown;
    }

    /// @brief Equality comparison.
    [[nodiscard]] constexpr auto operator==(const tribool b) noexcept
      -> tribool {
        return {
          _value == b._value,
          (not is(indeterminate) and not b.is(indeterminate))};
    }

    /// @brief Non-equality comparison.
    [[nodiscard]] constexpr auto operator!=(const tribool b) noexcept
      -> tribool {
        return {
          _value != b._value,
          (not is(indeterminate) and not b.is(indeterminate))};
    }

private:
    _value_t _value{_value_t::_false};
};

/// @brief Tri-state boolean and operator.
export [[nodiscard]] constexpr auto operator&&(
  const tribool a,
  const tribool b) noexcept {
    return not a   ? tribool{false}
           : a     ? b
           : not b ? tribool{false}
                   : tribool{indeterminate};
}

export [[nodiscard]] constexpr auto operator&&(
  const bool a,
  const tribool b) noexcept {
    return tribool(a) and b;
}

export [[nodiscard]] constexpr auto operator&&(
  const tribool a,
  const bool b) noexcept {
    return a and tribool(b);
}

/// @brief Tri-state boolean or operator.
export [[nodiscard]] constexpr auto operator||(
  const tribool a,
  const tribool b) noexcept {
    return a       ? tribool{true}
           : not a ? b
           : b     ? tribool{true}
                   : tribool{indeterminate};
}

export [[nodiscard]] constexpr auto operator||(
  const bool a,
  const tribool b) noexcept {
    return tribool(a) or b;
}

export [[nodiscard]] constexpr auto operator||(
  const tribool a,
  const bool b) noexcept {
    return a or tribool(b);
}

} // namespace eagine
