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
template <typename T>
constexpr auto adapt_placeholder_arg(T&& value)
  -> placeholder_terminal<std::remove_cvref_t<T>> {
    return {std::forward<T>(value)};
}
//------------------------------------------------------------------------------
template <std::size_t N>
constexpr auto adapt_placeholder_arg(const char (&value)[N])
  -> placeholder_terminal<std::string_view> {
    return {value};
}
//------------------------------------------------------------------------------
export template <typename F>
struct placeholder_expression;

template <typename F>
constexpr auto make_placeholder_expression(F&& f) noexcept
  -> placeholder_expression<std::remove_cvref_t<F>> {
    return {f};
}

template <typename Derived>
class placeholder_ops {
    auto derived() & noexcept -> Derived&& {
        return std::move(*static_cast<Derived*>(this));
    }

public:
    [[nodiscard]] constexpr auto operator*() noexcept {
        return make_placeholder_expression(
          [g{derived()}](auto&&... args) mutable -> decltype(auto) {
              return *(g(decltype(args)(args)...));
          });
    }

    [[nodiscard]] constexpr auto member(auto ptr)
        requires(std::is_member_object_pointer_v<decltype(ptr)>)
    {
        return make_placeholder_expression(
          [g{derived()}, ptr](auto&&... args) mutable -> auto& {
              return g(decltype(args)(args)...).*ptr;
          });
    }

    [[nodiscard]] constexpr auto member(auto ptr, auto&&... a)
        requires(std::is_member_function_pointer_v<decltype(ptr)>)
    {
        return make_placeholder_expression(
          [g{derived()}, ptr, ... exprs{adapt_placeholder_arg(decltype(a)(a))}](
            auto&&... args) mutable -> decltype(auto) {
              return (g(decltype(args)(args)...).*ptr)(
                exprs(decltype(args)(args)...)...);
          });
    }

    [[nodiscard]] constexpr auto empty() noexcept {
        return make_placeholder_expression(
          [g{derived()}](auto&&... args) mutable {
              return g(decltype(args)(args)...).empty();
          });
    }

    [[nodiscard]] constexpr auto size() noexcept {
        return make_placeholder_expression(
          [g{derived()}](auto&&... args) mutable {
              return g(decltype(args)(args)...).size();
          });
    }

    [[nodiscard]] constexpr auto has_value() noexcept {
        return make_placeholder_expression(
          [g{derived()}](auto&&... args) mutable -> bool {
              return g(decltype(args)(args)...).has_value();
          });
    }

    [[nodiscard]] constexpr auto value() noexcept {
        return make_placeholder_expression(
          [g{derived()}](auto&&... args) mutable {
              return g(decltype(args)(args)...).value();
          });
    }

    template <typename T>
    [[nodiscard]] constexpr auto value_or(T v) noexcept {
        return make_placeholder_expression(
          [g{derived()}, fallback{std::move(v)}](auto&&... args) mutable {
              return g(decltype(args)(args)...).value_or(std::move(fallback));
          });
    }

    [[nodiscard]] constexpr auto or_true() noexcept {
        return make_placeholder_expression(
          [g{derived()}](auto&&... args) mutable {
              return g(decltype(args)(args)...).or_true();
          });
    }

    [[nodiscard]] constexpr auto or_false() noexcept {
        return make_placeholder_expression(
          [g{derived()}](auto&&... args) mutable {
              return g(decltype(args)(args)...).or_false();
          });
    }

    template <typename X>
    [[nodiscard]] constexpr auto and_then(placeholder_expression<X> e) noexcept {
        return make_placeholder_expression(
          [g{derived()}, e](auto&&... args) mutable {
              return g(decltype(args)(args)...).and_then(e);
          });
    }

    [[nodiscard]] constexpr auto and_then(auto&& e) noexcept {
        return and_then(make_placeholder_expression(decltype(e)(e)));
    }

    template <typename T>
    [[nodiscard]] constexpr auto return_(T v) noexcept {
        return make_placeholder_expression(
          [g{derived()}, result{std::move(v)}](auto&&... args) mutable {
              (void)g(decltype(args)(args)...);
              return std::move(result);
          });
    }

    [[nodiscard]] constexpr auto return_true() noexcept {
        return make_placeholder_expression(
          [g{derived()}](auto&&... args) mutable -> tribool {
              (void)g(decltype(args)(args)...);
              return true;
          });
    }

    [[nodiscard]] constexpr auto return_false() noexcept {
        return make_placeholder_expression(
          [g{derived()}](auto&&... args) mutable -> tribool {
              (void)g(decltype(args)(args)...);
              return false;
          });
    }

    [[nodiscard]] constexpr auto to_tribool() noexcept {
        return make_placeholder_expression(
          [g{derived()}](auto&&... args) mutable {
              return tribool{g(decltype(args)(args)...)};
          });
    }

    [[nodiscard]] constexpr auto to_optional() noexcept {
        return make_placeholder_expression(
          [g{derived()}](auto&&... args) mutable {
              using R = decltype(g(decltype(args)(args)...));
              if constexpr(std::is_pointer_v<R>) {
                  return optional_reference<std::remove_pointer_t<R>>{
                    g(decltype(args)(args)...)};
              } else if constexpr(std::is_reference_v<R>) {
                  return optional_reference<std::remove_reference_t<R>>{
                    g(decltype(args)(args)...)};
              } else {
                  return std::optional<R>{g(decltype(args)(args)...)};
              }
          });
    }

    template <typename T>
    [[nodiscard]] constexpr auto cast_to() noexcept {
        return make_placeholder_expression(
          [g{derived()}](auto&&... args) mutable {
              return static_cast<T>(g(decltype(args)(args)...));
          });
    }
};

export template <typename F>
class placeholder_expression
  : public placeholder_ops<placeholder_expression<F>> {
    F _f;

    using Self = placeholder_expression;

public:
    constexpr placeholder_expression(F f) noexcept
      : _f{std::move(f)} {}

    constexpr placeholder_expression(Self&&) noexcept = default;
    constexpr placeholder_expression(const Self&) noexcept = default;
    constexpr auto operator=(Self&&) noexcept -> Self& = delete;
    constexpr auto operator=(const Self&) noexcept -> Self& = delete;
    constexpr ~placeholder_expression() noexcept = default;

    constexpr auto operator()(auto&&... args) noexcept(
      noexcept(std::declval<F>()(decltype(args)(args)...))) -> decltype(auto) {
        return _f(decltype(args)(args)...);
    }
};

export template <typename F>
placeholder_expression(F) -> placeholder_expression<F>;

export template <typename F>
constexpr auto adapt_placeholder_arg(placeholder_expression<F> expr)
  -> placeholder_expression<F> {
    return expr;
}
//------------------------------------------------------------------------------
template <std::size_t I>
using placeholder_i = std::integral_constant<std::size_t, I>;
//------------------------------------------------------------------------------
export template <>
struct placeholder_expression<placeholder_i<1>>
  : placeholder_ops<placeholder_expression<placeholder_i<1>>> {

    [[nodiscard]] constexpr auto bind(auto&& f) const noexcept {
        return make_placeholder_expression(decltype(f)(f));
    }

    constexpr auto operator()(auto&& arg1, auto&&...) const noexcept
      -> decltype(auto) {
        return decltype(arg1)(arg1);
    }
};
export constinit placeholder_expression<placeholder_i<1>> _1{};
//------------------------------------------------------------------------------
export template <>
struct placeholder_expression<placeholder_i<2>>
  : placeholder_ops<placeholder_expression<placeholder_i<2>>> {
    constexpr auto operator()(auto&&, auto&& arg2, auto&&...) const noexcept
      -> decltype(auto) {
        return decltype(arg2)(arg2);
    }
};
export constinit placeholder_expression<placeholder_i<2>> _2{};

export template <>
struct placeholder_expression<placeholder_i<3>>
  : placeholder_ops<placeholder_expression<placeholder_i<3>>> {
    constexpr auto operator()(auto&&, auto&&, auto&& arg3, auto&&...)
      const noexcept -> decltype(auto) {
        return decltype(arg3)(arg3);
    }
};
export constinit placeholder_expression<placeholder_i<3>> _3{};
//------------------------------------------------------------------------------
export template <typename X>
constexpr auto operator<<(
  std::ostream& out,
  placeholder_expression<X> e) noexcept {
    return placeholder_expression{
      [&out, e](auto&&... args) mutable -> std::ostream& {
          return out << e(decltype(args)(args)...);
      }};
}
//------------------------------------------------------------------------------
export template <typename X, typename R>
constexpr auto operator<<(placeholder_expression<X> l, R&& r) noexcept {
    return placeholder_expression{
      [l, re{adapt_placeholder_arg(decltype(r)(r))}](
        auto&&... args) mutable -> decltype(auto) {
          return l(decltype(args)(args)...) << re(decltype(args)(args)...);
      }};
}
//------------------------------------------------------------------------------
} // namespace eagine
