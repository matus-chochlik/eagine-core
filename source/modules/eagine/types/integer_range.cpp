/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.types:integer_range;

export import <ranges>;
import <concepts>;
import <type_traits>;

namespace eagine {

export template <std::integral I>
constexpr auto integer_range(I c) noexcept {
    return std::views::iota(I(0), c);
}

export template <std::integral B, std::integral E>
constexpr auto integer_range(B b, E e) noexcept {
    using I = std::common_type_t<B, E>;
    return std::views::iota(I(b), I(e));
}

} // namespace eagine

