/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.types:integer_range;

import std;
import :concepts;
import :limits;

namespace eagine {
//------------------------------------------------------------------------------
export template <std::integral T>
class integer {
public:
    constexpr integer() noexcept = default;
    constexpr integer(T value) noexcept
      : _value{value} {}

    [[nodiscard]] constexpr operator T() const noexcept {
        return _value;
    }

    template <std::integral I>
        requires(not std::is_same_v<T, I>)
    [[nodiscard]] constexpr operator I() const noexcept {
        assert(is_within_limits<I>(_value));
        return static_cast<I>(_value);
    }

private:
    T _value{};
};
//------------------------------------------------------------------------------
export template <std::integral I>
[[nodiscard]] constexpr auto integer_range(I c) noexcept {
    return std::ranges::iota_view(I(0), c);
}

export template <std::integral B, std::integral E>
[[nodiscard]] constexpr auto integer_range(B b, E e) noexcept {
    using I = std::common_type_t<B, E>;
    return std::ranges::iota_view(I(b), I(e));
}

export template <optional_like O>
[[nodiscard]] constexpr auto opt_integer_range(O c) noexcept {
    using I = extracted_type_t<O>;
    if(c) {
        return std::ranges::iota_view(I(0), *c);
    }
    return std::ranges::iota_view<I, I>();
}
//------------------------------------------------------------------------------
} // namespace eagine

