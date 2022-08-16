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

/// @brief Policy class for values valid if not equal to zero.
/// @ingroup valid_if
export template <typename T>
struct valid_if_nz_policy {

    /// @brief Indicates value validity, true if value != 0.
    constexpr auto operator()(const T value) const noexcept {
        return (value > T(0)) || (value < T(0));
    }

    struct do_log {
        template <does_not_hide<do_log> X>
        constexpr do_log(X&&) noexcept {}

        template <typename Log>
        void operator()(Log& log, const T&) const {
            log << "Value zero is invalid";
        }
    };
};

/// @brief Specialization of valid_if, for values valid if not equal to zero.
/// @ingroup valid_if
/// @see valid_if_not
/// @see nonzero_t
export template <typename T>
using valid_if_not_zero =
  valid_if<T, valid_if_nz_policy<std::remove_reference_t<T>>>;

/// @brief Alias for valid_if_not_zero.
/// @ingroup valid_if
export template <typename T>
using nonzero_t = valid_if_not_zero<T>;

/// @brief Policy class for values valid when they are positive.
/// @ingroup valid_if
export template <typename T>
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
export template <typename T>
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
export template <typename T, typename C, C Min, C Max>
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
export template <typename T, T Min, T Max>
using valid_if_between = valid_if_between_c<T, T, Min, Max>;

/// @brief Policy class for values valid if not equal to Cmp.
/// @ingroup valid_if
export template <typename T, T Cmp>
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

/// @brief Policy for values valid if between zero and one.
/// @ingroup valid_if
export template <typename T>
struct valid_if_ge0_le1_policy {

    /// @brief Indicates value validity, true if 0 <= value <= 1.
    constexpr auto operator()(const T value) const noexcept {
        return (T(0) <= value) && (value <= T(1));
    }

    struct do_log {
        template <does_not_hide<do_log> X>
        constexpr do_log(X&&) noexcept {}

        template <typename Log>
        void operator()(Log& log, const T& v) const {
            log << "Value " << v << ", "
                << "outside of interval [0,1] is invalid";
        }
    };
};

/// @brief Specialization of valid_if, for values valid if 0 <= value <= 1.
/// @ingroup valid_if
/// @see valid_if_between
/// @see valid_if_ge0_lt1
export template <typename T>
using valid_if_between_0_1 =
  valid_if<T, valid_if_ge0_le1_policy<std::remove_reference_t<T>>>;

/// @brief Policy for values valid if greate or equal to zero, less than one.
/// @ingroup valid_if
export template <typename T>
struct valid_if_ge0_lt1_policy {

    /// @brief Indicates value validity, true if 0 <= value < 1.
    constexpr auto operator()(const T value) const noexcept {
        return (T(0) <= value) && (value < T(1));
    }

    struct do_log {
        template <does_not_hide<do_log> X>
        constexpr do_log(X&&) noexcept {}

        template <typename Log>
        void operator()(Log& log, const T& v) const {
            log << "Value " << v << ", "
                << "outside of interval [0,1) is invalid";
        }
    };
};

/// @brief Specialization of valid_if, for values valid if 0 <= value < 1.
/// @ingroup valid_if
/// @see valid_if_between
/// @see valid_if_between_0_1
export template <typename T>
using valid_if_ge0_lt1 =
  valid_if<T, valid_if_ge0_lt1_policy<std::remove_reference_t<T>>>;

export template <typename T>
struct valid_if_gt0_lt1_policy {
    constexpr auto operator()(const T value) const noexcept {
        return (T(0) < value) && (value < T(1));
    }

    struct do_log {
        template <does_not_hide<do_log> X>
        constexpr do_log(X&&) noexcept {}

        template <typename Log>
        void operator()(Log& log, const T& v) const {
            log << "Value " << v << ", "
                << "outside of interval (0,1) is invalid";
        }
    };
};

/// @brief Specialization of valid_if, for values valid if 0 < value < 1.
/// @ingroup valid_if
/// @see valid_if_between
/// @see valid_if_between_0_1
export template <typename T>
using valid_if_gt0_lt1 =
  valid_if<T, valid_if_gt0_lt1_policy<std::remove_reference_t<T>>>;

/// @brief Policy class for values valid if greater than Cmp.
/// @ingroup valid_if
export template <typename T, T Cmp>
struct valid_if_gt_policy {
    constexpr auto operator()(const T value) const noexcept {
        return value > Cmp;
    }

    struct do_log {
        template <does_not_hide<do_log> X>
        constexpr do_log(X&&) noexcept {}

        template <typename Log>
        void operator()(Log& log, const T& v) const {
            log << "Value " << v << ", "
                << "less then or equal to " << Cmp << " "
                << "is invalid";
        }
    };
};

/// @brief Specialization of valid_if, for values valid if greater than Cmp.
/// @ingroup valid_if
/// @see valid_if_between
export template <typename T, T Cmp>
using valid_if_greater_than =
  valid_if<T, valid_if_gt_policy<std::remove_reference_t<T>, Cmp>>;

/// @brief Policy class for values valid if less than Cmp.
/// @ingroup valid_if
export template <typename T, T Cmp>
struct valid_if_lt_policy {

    /// @brief Indicates value validity, true if value < Cmp.
    constexpr auto operator()(const T value) const noexcept {
        return value < Cmp;
    }

    struct do_log {
        template <does_not_hide<do_log> X>
        constexpr do_log(X&&) noexcept {}

        template <typename Log>
        void operator()(Log& log, const T& v) const {
            log << "Value " << v << ", "
                << "greater then or equal to " << Cmp << " "
                << "is invalid";
        }
    };
};

/// @brief Specialization of valid_if, for values valid if less than Cmp.
/// @ingroup valid_if
/// @see valid_if_between
/// @see valid_if_lt_size
export template <typename T, T Cmp>
using valid_if_less_than =
  valid_if<T, valid_if_lt_policy<std::remove_reference_t<T>, Cmp>>;

template <
  typename T,
  typename C,
  typename Policy,
  typename DoLog = typename Policy::do_log>
using in_class_valid_if = basic_valid_if<T, Policy, DoLog, const C&>;

/// @brief Policy class for containers valid if their size is larger than specified number.
/// @ingroup valid_if
export template <typename C, typename T>
struct valid_if_size_gt_policy {

    /// @brief Indicates value validity, true if c.size() > s.
    constexpr auto operator()(const C& c, const T s) const {
        return c.size() > s;
    }

    struct do_log {
        template <does_not_hide<do_log> X>
        constexpr do_log(X&&) noexcept {}

        template <typename Log>
        void operator()(Log& log, const C& c, const T& s) const {
            log << "Size " << c.size() << ", "
                << "not greater than value " << s << " is invalid";
        }
    };
};

/// @brief Specialization of valid_if, for values valid if they contain more than n items.
/// @ingroup valid_if
/// @see valid_if_not_empty
export template <typename C, typename T>
using valid_if_size_gt = in_class_valid_if<C, T, valid_if_size_gt_policy<C, T>>;

/// @brief Policy for values valid if >= 0 and < container.size().
/// @ingroup valid_if
export template <typename T, typename C>
struct valid_if_lt_size_ge0_policy {

    /// @brief Indicates value validity, true if 0 <= x < c.size().
    auto operator()(const T x, const C& c) const {
        return (T(0) <= x) && (x < c.size());
    }

    /// @brief Indicates value validity, true if 0 <= x < c.size() - o.
    auto operator()(const T x, const C& c, const T o) const {
        return (T(0) <= x) && (x < c.size() - o);
    }

    struct do_log {
        template <does_not_hide<do_log> X>
        constexpr do_log(X&&) noexcept {}

        template <typename Log>
        void operator()(Log& log, const T& v, const C& c) const {
            log << "Value " << v << ", less than zero or "
                << "not less than c.size() = " << c.size() << " is invalid";
        }
    };
};

/// @brief Specialization of valid_if, for values valid if >= 0 and < container.size().
/// @ingroup valid_if
/// @see valid_if_le_size_ge0
export template <typename C, typename T>
using valid_if_lt_size_ge0 =
  in_class_valid_if<T, C, valid_if_lt_size_ge0_policy<T, C>>;

/// @brief Policy for values valid if >= 0 and <= container.size().
/// @ingroup valid_if
export template <typename T, typename C>
struct valid_if_le_size_ge0_policy {

    /// @brief Indicates value validity, true if 0 <= x <= c.size().
    auto operator()(const T x, const C& c) const {
        return (T(0) <= x) && (x <= c.size());
    }

    /// @brief Indicates value validity, true if 0 <= x <= c.size() - o.
    auto operator()(const T x, const C& c, const T o) const {
        return (T(0) <= x) && (x <= c.size() - o);
    }

    struct do_log {
        template <does_not_hide<do_log> X>
        constexpr do_log(X&&) noexcept {}

        template <typename Log>
        void operator()(Log& log, const T& v, const C& c) const {
            log << "Value " << v << ", less than zero or "
                << "greater than c.size() = " << c.size() << " is invalid";
        }
    };
};

/// @brief Specialization of valid_if, for values valid if >= 0 and <= container.size().
/// @ingroup valid_if
/// @see valid_if_lt_size_ge0
export template <typename C, typename T>
using valid_if_le_size_ge0 =
  in_class_valid_if<T, C, valid_if_le_size_ge0_policy<T, C>>;

} // namespace eagine
