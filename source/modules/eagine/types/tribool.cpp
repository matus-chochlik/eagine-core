/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.types:tribool;

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
    constexpr explicit operator bool() const noexcept {
        return _value != _value_t::_false;
    }

    /// @brief Returns true, if the stored value is not @c true.
    constexpr auto operator!() const noexcept {
        return _value != _value_t::_true;
    }

    ///@ brief Checks if the stored value is indeterminate.
    constexpr auto is(const indeterminate_t) const noexcept {
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
    /// @brief Default constructor.
    constexpr tribool() noexcept = default;

    /// @brief Constructions from boolean value.
    constexpr tribool(const bool value) noexcept
      : _value{value ? _value_t::_true : _value_t::_false} {}

    /// @brief Constructions from indeterminate value.
    constexpr tribool(const indeterminate_t) noexcept
      : _value{_value_t::_unknown} {}

    /// @brief Construction with separate true/false, known/unknown arguments.
    constexpr tribool(const bool value, const bool is_unknown) noexcept
      : _value(
          is_unknown ? _value_t::_unknown
          : value    ? _value_t::_true
                     : _value_t::_false) {}

    /// @brief Returns true, if the stored value is true.
    constexpr explicit operator bool() const noexcept {
        return _value == _value_t::_true;
    }

    /// @brief Returns true, if the stored value is false.
    constexpr auto operator!() const noexcept {
        return _value == _value_t::_false;
    }

    /// @brief Returns true, if the stored value is indeterminate.
    constexpr auto operator*() const noexcept -> bool {
        return _value == _value_t::_unknown;
    }

    /// @brief Converts this value to @c weakbool.
    constexpr auto operator~() const noexcept -> weakbool {
        return weakbool{_value};
    }

    /// @brief Returns true if the stored value is known and equal to @p value.
    constexpr auto is(const bool value) const noexcept -> bool {
        return _value == (value ? _value_t::_true : _value_t::_false);
    }

    /// @brief Returns true if the stored value is indeterminate.
    constexpr auto is(const indeterminate_t) const noexcept -> bool {
        return *(*this);
    }

    /// @brief Equality comparison.
    constexpr auto operator==(const tribool b) noexcept -> tribool {
        return {_value == b._value, (*(*this) || *b)};
    }

    /// @brief Non-equality comparison.
    constexpr auto operator!=(const tribool b) noexcept -> tribool {
        return {_value != b._value, (*(*this) || *b)};
    }

private:
    _value_t _value{_value_t::_false};
};

/// @brief Tri-state boolean and operator.
export constexpr auto operator&&(const tribool a, const tribool b) noexcept {
    return !a   ? tribool{false}
           : a  ? b
           : !b ? tribool{false}
                : tribool{indeterminate};
}

export constexpr auto operator&&(const bool a, const tribool b) noexcept {
    return tribool(a) && b;
}

export constexpr auto operator&&(const tribool a, const bool b) noexcept {
    return a && tribool(b);
}

/// @brief Tri-state boolean or operator.
export constexpr auto operator||(const tribool a, const tribool b) noexcept {
    return a    ? tribool{true}
           : !a ? b
           : b  ? tribool{true}
                : tribool{indeterminate};
}

export constexpr auto operator||(const bool a, const tribool b) noexcept {
    return tribool(a) || b;
}

export constexpr auto operator||(const tribool a, const bool b) noexcept {
    return a || tribool(b);
}

} // namespace eagine
