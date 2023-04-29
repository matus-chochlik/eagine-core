/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.types:placeholder;

import std;
import :tribool;
import :optional_reference;

namespace eagine {
//------------------------------------------------------------------------------
export template <typename T>
struct placeholder_terminal {
    T value;

    constexpr auto operator()(auto&&...) const noexcept -> const T& {
        return value;
    }
};
//------------------------------------------------------------------------------
export template <typename T>
consteval auto adapt_placeholder_arg(T&& value)
  -> placeholder_terminal<std::remove_cvref_t<T>> {
    return {std::forward<T>(value)};
}
//------------------------------------------------------------------------------
export template <typename F>
struct placeholder_expression {
    F f;

    constexpr auto operator()(auto&&... args) noexcept(
      noexcept(std::declval<F>()(decltype(args)(args)...))) -> decltype(auto) {
        return f(decltype(args)(args)...);
    }

    constexpr auto operator()(auto&&... args) const
      noexcept(noexcept(std::declval<const F>()(decltype(args)(args)...)))
        -> decltype(auto) {
        return f(decltype(args)(args)...);
    }
};

template <typename F>
consteval auto make_placeholder_expression(F&& f) noexcept
  -> placeholder_expression<std::remove_cvref_t<F>> {
    return {f};
}

export template <typename F>
placeholder_expression(F) -> placeholder_expression<F>;

export template <typename F>
consteval auto adapt_placeholder_arg(placeholder_expression<F> expr)
  -> placeholder_expression<F> {
    return expr;
}
//------------------------------------------------------------------------------
export template <>
struct placeholder_expression<std::integral_constant<size_t, 1>> {
    consteval auto member(auto ptr) const
        requires(std::is_member_object_pointer_v<decltype(ptr)>)
    {
        return make_placeholder_expression(
          [ptr](auto&& self, auto&&...) { return decltype(self)(self).*ptr; });
    }

    consteval auto optional(auto ptr) const
        requires(std::is_member_object_pointer_v<decltype(ptr)>)
    {
        return make_placeholder_expression([ptr](auto&& self, auto&&...) {
            return optional_reference(decltype(self)(self).*ptr);
        });
    }

    consteval auto member(auto ptr, auto&&... args) const
        requires(std::is_member_function_pointer_v<decltype(ptr)>)
    {
        return make_placeholder_expression(
          [ptr, ... exprs{adapt_placeholder_arg(decltype(args)(args))}](
            auto&& self, const auto&... as) {
              return (decltype(self)(self).*ptr)(exprs(as...)...);
          });
    }

    consteval auto optional(auto ptr, auto&&... args) const
        requires(std::is_member_function_pointer_v<decltype(ptr)>)
    {
        return make_placeholder_expression(
          [ptr, ... exprs{adapt_placeholder_arg(decltype(args)(args))}](
            auto&& self, const auto&... as) {
              using R = decltype((decltype(self)(self).*ptr)(exprs(as...)...));
              if constexpr(std::is_pointer_v<R>) {
                  return optional_reference<std::remove_pointer_t<R>>{
                    (decltype(self)(self).*ptr)(exprs(as...)...)};
              } else if constexpr(std::is_reference_v<R>) {
                  return optional_reference<std::remove_reference_t<R>>{
                    (decltype(self)(self).*ptr)(exprs(as...)...)};
              } else {
                  return std::optional<R>{
                    (decltype(self)(self).*ptr)(exprs(as...)...)};
              }
          });
    }

    constexpr auto operator()(auto&& arg1, auto&&...) const noexcept
      -> decltype(auto) {
        return decltype(arg1)(arg1);
    }
};
export constinit const placeholder_expression<std::integral_constant<size_t, 1>>
  _1{};
//------------------------------------------------------------------------------
export template <typename X>
consteval auto to_optional(placeholder_expression<X> e) noexcept {
    return placeholder_expression{[e](auto&&... args) {
        return std::optional{e(decltype(args)(args)...)};
    }};
}

export template <typename X>
consteval auto to_tribool(placeholder_expression<X> e) noexcept {
    return placeholder_expression{[e](auto&&... args) {
        return tribool{e(decltype(args)(args)...)};
    }};
}
//------------------------------------------------------------------------------
} // namespace eagine
