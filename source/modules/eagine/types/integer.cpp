/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.types:integer;

import :limits;
import <concepts>;
import <type_traits>;

namespace eagine {

template <std::integral T>
class integer {
public:
    constexpr integer() noexcept = default;
    constexpr integer(T value) noexcept
      : _value{value} {}

    constexpr operator T() const noexcept {
        return _value;
    }

    template <std::integral I>
        requires(!std::is_same_v<T, I>)
    constexpr operator I() const noexcept {
        assert(is_within_limits<I>(_value));
        return static_cast<I>(_value);
    }

private:
    T _value{};
};

} // namespace eagine
