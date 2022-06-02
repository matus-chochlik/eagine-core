/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.valid_if:compare;

import eagine.core.concepts;
import :decl;
import <type_traits>;

namespace eagine {

/// @brief Policy class for values valid when they are positive.
/// @ingroup valid_if
template <typename T>
struct valid_if_positive_policy {

    /// @brief Indicates value validity, true if 0 < value.
    constexpr auto operator()(const T value) const noexcept {
        return value > T(0);
    }

    struct do_log {
        template <does_not_hide<do_log> X>
        constexpr do_log(X&&) noexcept {}

        template <typename Log>
        void operator()(Log& log, const T& v) const {
            log << "Value " << v << ", "
                << "less then or equal to zero "
                << "is invalid";
        }
    };
};

/// @brief Specialization of valid_if, for values valid if positive.
/// @ingroup valid_if
/// @see valid_if_nonnegative
/// @see valid_if_less_than
export template <typename T>
using valid_if_positive =
  valid_if<T, valid_if_positive_policy<std::remove_reference_t<T>>>;

/// @brief Policy class for values valid when they are non-negative.
/// @ingroup valid_if
template <typename T>
struct valid_if_nonneg_policy {

    /// @brief Indicates value validity, true if 0 <= value.
    constexpr auto operator()(const T value) const noexcept {
        return value >= T(0);
    }

    struct do_log {
        template <does_not_hide<do_log> X>
        constexpr do_log(X&&) noexcept {}

        template <typename Log>
        void operator()(Log& log, const T& v) const {
            log << "Value " << v << ", "
                << "less then zero "
                << "is invalid";
        }
    };
};

/// @brief Specialization of valid_if, for values valid if non-negative.
/// @ingroup valid_if
/// @see valid_if_positive
/// @see valid_if_less_than
export template <typename T>
using valid_if_nonnegative =
  valid_if<T, valid_if_nonneg_policy<std::remove_reference_t<T>>>;

/// @brief Policy for values valid if between Min and Max.
/// @tparam T the checked value type must be convertible to @p C.
/// @tparam C the Min and Max constant type.
/// @tparam Min the minimal valid value.
/// @tparam Max the maximal valid value.
/// @ingroup valid_if
template <typename T, typename C, C Min, C Max>
struct valid_if_btwn_policy {

    /// @brief Indicates @p value validity, true if between Min and Max.
    constexpr auto operator()(const T value) const noexcept {
        return (T(Min) <= value) && (value <= T(Max));
    }

    struct do_log {
        template <does_not_hide<do_log> X>
        constexpr do_log(X&&) noexcept {}

        template <typename Log>
        void operator()(Log& log, const T& v) const {
            log << "Value " << v << ", not between " << Min << " and " << Max
                << " is invalid";
        }
    };
};

/// @brief Specialization of valid_if, for values valid if between Min and Max.
/// @ingroup valid_if
export template <typename T, typename C, C Min, C Max>
using valid_if_between_c =
  valid_if<T, valid_if_btwn_policy<std::remove_reference_t<T>, C, Min, Max>>;

/// @brief Specialization of valid_if, for values valid if between Min and Max.
/// @ingroup valid_if
/// @see valid_if_between_0_1
/// @see valid_if_less_than
/// @see valid_if_within_limits
template <typename T, T Min, T Max>
using valid_if_between = valid_if_between_c<T, T, Min, Max>;

/// @brief Policy class for values valid if not equal to Cmp.
/// @ingroup valid_if
template <typename T, T Cmp>
struct valid_if_ne_policy {

    /// @brief Indicates value validity, true if value != Cmp.
    constexpr auto operator()(const T value) const noexcept {
        return value != Cmp;
    }

    struct do_log {
        template <does_not_hide<do_log> X>
        constexpr do_log(X&&) noexcept {}

        template <typename Log>
        void operator()(Log& log, const T&) const {
            log << "Value equal to " << Cmp << " is invalid";
        }
    };
};

/// @brief Specialization of valid_if, for values valid if not equal to Cmp.
/// @ingroup valid_if
/// @see valid_if_not_zero
export template <typename T, T Cmp>
using valid_if_not =
  valid_if<T, valid_if_ne_policy<std::remove_reference_t<T>, Cmp>>;

} // namespace eagine
