/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.valid_if:not_empty;

import eagine.core.concepts;
import :decl;
import <type_traits>;

namespace eagine {

/// @brief Policy class for containers valid if their empty() member function return false.
/// @ingroup valid_if
export template <typename T>
struct valid_if_not_empty_policy {

    /// @brief Indicates value validity, true if !range.empty().
    constexpr auto operator()(const T& range) const noexcept {
        return !range.empty();
    }

    struct do_log {
        template <does_not_hide<do_log> X>
        constexpr do_log(X&&) noexcept {}

        template <typename Log>
        void operator()(Log& log, const T&) const {
            log << "Empty range, string or container is invalid";
        }
    };
};

/// @brief Specialization of valid_if, for values valid if not empty.
/// @ingroup valid_if
/// @see valid_if_size_gt
/// @see valid_if_lt_size
export template <typename T>
using valid_if_not_empty =
  valid_if<T, valid_if_not_empty_policy<std::remove_reference_t<T>>>;

} // namespace eagine
