/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.types:placeholder;

import std;

namespace eagine {
//------------------------------------------------------------------------------
export template <typename F>
struct placeholder_expression {
    F f;

    template <typename... Args>
    constexpr auto operator()(Args&&... args) noexcept(noexcept(
      std::declval<F>()(std::forward<Args>(args)...))) -> decltype(auto) {
        return f(std::forward<Args>(args)...);
    }

    template <typename... Args>
    constexpr auto operator()(Args&&... args) const
      noexcept(noexcept(std::declval<const F>()(std::forward<Args>(args)...)))
        -> decltype(auto) {
        return f(std::forward<Args>(args)...);
    }
};

export template <typename F>
placeholder_expression(F) -> placeholder_expression<F>;

export template <>
struct placeholder_expression<std::integral_constant<size_t, 1>> {
    template <typename Arg1, typename... Args>
    constexpr auto operator()(Arg1&& arg1, Args&...) const noexcept
      -> decltype(auto) {
        return std::forward<Arg1>(arg1);
    }
};
export constinit const placeholder_expression<std::integral_constant<size_t, 1>>
  _1{};
//------------------------------------------------------------------------------
} // namespace eagine
