/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_UNITS_DEFAULT_HPP
#define EAGINE_UNITS_DEFAULT_HPP

#include "fwd.hpp"
#include <type_traits>

namespace eagine::units {

template <typename X, typename Y>
struct same_dimension : std::false_type {};

template <typename U>
struct add_result<U, U> : std::type_identity<U> {};

template <typename U>
struct sub_result<U, U> : std::type_identity<U> {};

template <typename T1, typename T2>
struct is_convertible : std::false_type {};

template <typename U>
struct is_convertible<U, U> : std::true_type {};

struct trivial_value_conv {
    template <typename T>
    constexpr auto operator()(const T v) const -> T {
        return v;
    }
};

template <typename U>
struct value_conv<U, U> : trivial_value_conv {};

} // namespace eagine::units

#endif // EAGINE_UNITS_DEFAULT_HPP
