/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.valid_if:indicated;

import eagine.core.concepts;
import :decl;
import <utility>;

namespace eagine {

/// @brief Policy for values valid when non-boolean Indicator has Value.
/// @ingroup valid_if
export template <typename Indicator, typename Comparable, Comparable Value>
struct valid_if_indicated_policy {
    Indicator _indicator{};

    constexpr valid_if_indicated_policy() noexcept = default;

    constexpr valid_if_indicated_policy(Indicator indicator) noexcept
      : _indicator{std::move(indicator)} {}

    /// @brief Indicates value validity, true if indicator == Value.
    template <typename T>
    auto operator()(const T&) const noexcept {
        return Comparable(_indicator) == Value;
    }

    struct do_log {
        template <does_not_hide<do_log> X>
        constexpr do_log(X&&) noexcept {}

        template <typename Log, typename T>
        void operator()(Log& log, const T&) const {
            log << "indicator is " << Value;
        }
    };
};

/// @brief Specialization of valid_if, for values with non-boolean indicator.
/// @ingroup valid_if
/// @see optionally_valid
///
/// This is a more generalized for of optionally_valid for indicators of types
/// other than boolean.
export template <
  typename T,
  typename Indicator,
  typename Comparable = bool,
  Comparable Value = true>
using valid_if_indicated =
  valid_if<T, valid_if_indicated_policy<Indicator, Comparable, Value>>;
//------------------------------------------------------------------------------
} // namespace eagine

