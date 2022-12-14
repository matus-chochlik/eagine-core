/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.utility:bool_aggregate;

namespace eagine {

/// @brief Class storing initially false value and logically or-ing other values.
/// @ingroup type_utils
export class some_true {
public:
    /// @brief Default constructible.
    constexpr some_true() noexcept = default;
    /// @brief Initializing constructior.
    constexpr some_true(const bool init) noexcept
      : _result{init} {}

    /// @brief Logically or-s the stored state with the specified argument.
    constexpr auto operator()(const bool b = true) noexcept -> auto& {
        _result |= b;
        return *this;
    }

    /// @brief Logically or-s the stored state with the specified argument.
    constexpr auto operator()(const some_true that) noexcept -> auto& {
        _result |= that._result;
        return *this;
    }

    /// @brief Returns the current boolean state.
    [[nodiscard]] constexpr explicit operator bool() const noexcept {
        return _result;
    }

private:
    bool _result{false};
};

/// @brief Type alias used as return value indicating that some work was done.
/// @ingroup type_utils
export using work_done = some_true;

} // namespace eagine
