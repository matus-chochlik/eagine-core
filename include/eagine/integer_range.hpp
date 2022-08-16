/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_INTEGER_RANGE_HPP
#define EAGINE_INTEGER_RANGE_HPP

#include <concepts>
#include <ranges>

namespace eagine {

template <std::integral I>
constexpr auto integer_range(I c) noexcept {
    return std::ranges::iota_view(I(0), c);
}

template <std::integral B, std::integral E>
constexpr auto integer_range(B b, E e) noexcept {
    using I = std::common_type_t<B, E>;
    return std::ranges::iota_view(I(b), I(e));
}

} // namespace eagine

#endif // EAGINE_INTEGER_RANGE_HPP
