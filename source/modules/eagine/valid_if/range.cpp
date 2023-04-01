/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.valid_if:range;

import std;
import eagine.core.concepts;
import eagine.core.types;
import :decl;
import :compare;

namespace eagine {
//------------------------------------------------------------------------------
template <typename R>
struct range_index_type {
    using type = typename R::size_type;
};
//------------------------------------------------------------------------------
template <typename R>
using range_index_t = typename range_index_type<R>::type;
//------------------------------------------------------------------------------
export template <typename R>
using any_range_position = valid_if_nonnegative<range_index_t<R>>;
//------------------------------------------------------------------------------
export template <typename R>
using valid_range_index = valid_if_lt_size_ge0<R, range_index_t<R>>;
//------------------------------------------------------------------------------
export template <typename R>
using valid_range_position = valid_if_le_size_ge0<R, range_index_t<R>>;
//------------------------------------------------------------------------------
export template <typename R, typename T>
constexpr auto range_index(const T i) noexcept {
    return limit_cast<range_index_t<R>>(i);
}
//------------------------------------------------------------------------------
export template <typename T, T... C>
struct valid_if_one_of_policy {
    auto operator()(const T& value) const noexcept {
        const T choices[] = {C...};
        for(const T& choice : choices) {
            if(value == choice) {
                return true;
            }
        }
        return false;
    }

    struct do_log {
        template <does_not_hide<do_log> X>
        constexpr do_log(X&&) noexcept {}

        template <typename Log>
        void operator()(Log& log, const T& v) const {
            log << "Value " << v << ", "
                << "other than one of the values [";

            const T choices[] = {C...};
            bool first = true;
            for(const T& choice : choices) {
                log << (first ? "" : ", ") << choice;
                first = false;
            }
            log << "] is invalid";
        }
    };
};

export template <typename T, auto... C>
using valid_if_one_of =
  valid_if<T, valid_if_one_of_policy<std::remove_reference_t<T>, C...>>;
//------------------------------------------------------------------------------
} // namespace eagine

