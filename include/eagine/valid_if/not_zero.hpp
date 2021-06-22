/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_VALID_IF_NOT_ZERO_HPP
#define EAGINE_VALID_IF_NOT_ZERO_HPP

#include "decl.hpp"

namespace eagine {

/// @brief Policy class for values valid if not equal to zero.
/// @ingroup valid_if
template <typename T>
struct valid_if_nz_policy {

    /// @brief Indicates value validity, true if value != 0.
    constexpr auto operator()(T value) const noexcept {
        return (value > T(0)) || (value < T(0));
    }

    struct do_log {
        template <typename X, typename = disable_if_same_t<X, do_log>>
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
template <typename T>
using valid_if_not_zero = valid_if<T, valid_if_nz_policy<T>>;

/// @brief Alias for valid_if_not_zero.
/// @ingroup valid_if
template <typename T>
using nonzero_t = valid_if_not_zero<T>;

template <typename T>
static inline auto nonzero(T v) noexcept -> nonzero_t<T> {
    return {v};
}

} // namespace eagine

#endif // EAGINE_VALID_IF_NOT_ZERO_HPP
