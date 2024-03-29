/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.valid_if:range;

import std;
import eagine.core.types;
import :compare;

namespace eagine {
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

