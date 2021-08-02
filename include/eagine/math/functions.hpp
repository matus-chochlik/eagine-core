/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#ifndef EAGINE_MATH_FUNCTIONS_HPP
#define EAGINE_MATH_FUNCTIONS_HPP

#include "../memory/span.hpp"
#include "../valid_if/decl.hpp"
#include "constants.hpp"
#include <cassert>
#include <cmath>
#include <type_traits>

namespace eagine::math {
//------------------------------------------------------------------------------
/// @brief Indicates if @p value is a positive integral power of two.
/// @ingroup math
template <typename T>
static constexpr auto is_positive_power_of_2(const T value) noexcept
  -> std::enable_if_t<std::is_integral_v<T>, bool> {
    using U = std::make_unsigned_t<T>;
    return (value > 0) && ((U(value) & (U(value) - 1)) == 0);
}
//------------------------------------------------------------------------------
/// @brief Returns the greates common divisor of arguments @p l and @p r.
/// @ingroup math
template <typename T>
static constexpr auto greatest_common_divisor(const T l, const T r) noexcept
  -> std::enable_if_t<std::is_integral_v<T>, T> {
    return (r == T(0)) ? l : greatest_common_divisor(r, T(l % r));
}
//------------------------------------------------------------------------------
/// @brief Returns 1 if @p x is non-negative, returns -1 otherwise.
/// @ingroup math
template <typename T>
static constexpr auto signum(const T x) noexcept {
    return (x < 0) ? T(-1) : T(1);
}
//------------------------------------------------------------------------------
/// @brief Returns the minimum value of @p a and @p b.
/// @ingroup math
template <typename T>
static constexpr auto minimum(const T a, const T b) noexcept {
    return a < b ? a : b;
}
//------------------------------------------------------------------------------
/// @brief Returns the minimum value from multiple arguments.
/// @ingroup math
template <typename T, typename... P>
static constexpr auto minimum(
  const T a,
  const T b,
  const T c,
  const P... d) noexcept {
    return minimum(minimum(a, b), c, d...);
}
//------------------------------------------------------------------------------
/// @brief Returns the maximum value of @p a and @p b.
/// @ingroup math
template <typename T>
static constexpr auto maximum(const T a, const T b) noexcept {
    return a > b ? a : b;
}
//------------------------------------------------------------------------------
/// @brief Returns the maximum value from multiple arguments.
/// @ingroup math
template <typename T, typename... P>
static constexpr auto maximum(
  const T a,
  const T b,
  const T c,
  const P... d) noexcept {
    return maximum(maximum(a, b), c, d...);
}
//------------------------------------------------------------------------------
/// @brief Returns @p a divided by @p b if @p b is not zero.
/// @ingroup math
template <typename T>
static constexpr auto ratio(const T a, const T b) noexcept
  -> optionally_valid<T> {
    if(b > T(0) || (b < T(0))) {
        return {a / b, true};
    }
    return {};
}
//------------------------------------------------------------------------------
/// @brief Returns the reciprocal of @p x if @p x is not zero.
/// @ingroup math
template <typename T>
static constexpr auto reciprocal(const T x) noexcept -> optionally_valid<T> {
    using std::abs;
    if(abs(x) > std::numeric_limits<T>::epsilon()) {
        return {T(1) / x, true};
    }
    return {};
}
//------------------------------------------------------------------------------
/// @brief Clamps @p x to be between @p min and @p max.
/// @ingroup math
template <typename T, typename Min, typename Max>
static constexpr auto clamp(const T x, const Min min, const Max max) noexcept {
    return x < T(min) ? T(min) : x > T(max) ? T(max) : x;
}
//------------------------------------------------------------------------------
/// @brief Normalizes @p x to (0, 1), where @p start = 0 and @p end = 1.
/// @ingroup math
template <typename T, typename S, typename E>
static constexpr auto ramp(const T x, const S start, const E end) noexcept {
    return (clamp(x, start, end) - start) / (end - start);
}
//------------------------------------------------------------------------------
/// @brief Blends @p v1 and @p v2, using @p alpha as the blending factor.
/// @ingroup math
template <typename T, typename A>
static constexpr auto blend(const T v1, const T v2, const A alpha) noexcept {
    return v1 * alpha + v2 * (1 - alpha);
}
//------------------------------------------------------------------------------
/// @brief Calculates the inverse logistic (log(x) - log(1 - x)) of @p x.
/// @ingroup math
template <typename T>
static constexpr auto inverse_logistic(const T x) noexcept {
    using std::log;
    return log(x) - log(1 - x);
}
//------------------------------------------------------------------------------
/// @brief Calculates the logistic (1 / (1 + exp(-x))) of @p x.
/// @ingroup math
template <typename T>
static constexpr auto logistic(const T x) noexcept {
    using std::exp;
    return 1 / (1 + exp(-x));
}
//------------------------------------------------------------------------------
/// @brief Calculates the sigmoid of @p x. The value @p c controls steepness.
/// @ingroup math
/// @pre 0 <= x <= 1
template <typename T, typename C>
static constexpr auto sigmoid01(const T x, const C c) noexcept {
    return logistic(c * inverse_logistic(x));
}
//------------------------------------------------------------------------------
/// @brief Calculates the default sigmoid of @p x.
/// @ingroup math
/// @pre 0 <= x <= 1
template <typename T>
static constexpr auto sigmoid01(const T x) noexcept {
    return sigmoid01(x, 2);
}
//------------------------------------------------------------------------------
/// @brief Calculates goniometric sigmoid (cos in interval (0, 1)) of @p x.
/// @ingroup math
/// @pre 0 <= x <= 1
template <typename T>
static inline auto sine_sigmoid01(const T x) {

#ifdef __clang__
    EAGINE_DIAG_PUSH()
    EAGINE_DIAG_OFF(double-promotion)
#endif
    using std::cos;
    return (1 - cos(x * pi)) / 2;
#ifdef __clang__
    EAGINE_DIAG_POP()
#endif
}
//------------------------------------------------------------------------------
/// @brief Calculates sine of @p x, mapped to interval (0, 1).
/// @ingroup math
/// @pre 0 <= x <= 1
template <typename T>
static constexpr auto sine_wave01(const T x) noexcept {
    using std::sin;
    return (sin(2 * pi * x) + 1) / 2;
}
//------------------------------------------------------------------------------
/// @brief Calculates cosine of @p x, mapped to interval (0, 1).
/// @ingroup math
/// @pre 0 <= x <= 1
template <typename T>
static constexpr auto cosine_wave01(const T x) noexcept {
    using std::cos;
    return (cos(2 * pi * x) + 1) / 2;
}
//------------------------------------------------------------------------------
/// @brief Calculates floating-point modulo of @p x in intervals of @p u.
/// @ingroup math
template <typename T, typename U = T>
static constexpr auto saw(const T x, const U u = T(1)) noexcept {
    using std::fmod;
    return fmod(x, u);
}
//------------------------------------------------------------------------------
/// @brief Calculates factorial of @p n.
/// @ingroup math
template <typename T>
static constexpr auto factorial(const T n) noexcept
  -> std::enable_if_t<std::is_integral_v<T>, T> {
    return n > 0 ? n * factorial(n - 1) : 1;
}
//------------------------------------------------------------------------------
/// @brief Calculates binomial coefficient of @p n over @p k.
/// @ingroup math
template <typename T>
static constexpr auto binomial(const T n, const T k) noexcept
  -> std::enable_if_t<std::is_integral_v<T>, T> {
    return ((n >= 0) && (k >= 0) && (k <= n))
             ? (factorial(n) / (factorial(k) * factorial(n - k)))
             : 0;
}
//------------------------------------------------------------------------------
/// @brief Helper class for bezier curve segment calculations.
/// @ingroup math
/// @tparam Type the interpolated type.
/// @tparam Parameter the curve interpolation parameter type.
/// @tparam N the curve order.
/// @see bezier_curves
template <typename Type, typename Parameter, int N>
struct bezier_t {
public:
    /// @brief Interpolate from control points in pack @p p at position @p t.
    template <
      typename... Points,
      typename = std::enable_if_t<sizeof...(Points) == N>>
    constexpr auto operator()(const Parameter t, Points&&... p) const noexcept {
        return _calc(N - 1, 0, t, std::forward<Points>(p)...);
    }

    /// @brief Interpolate from control points in span @p p at position @p t.
    template <typename P, typename S>
    auto operator()(const Parameter t, memory::basic_span<const Type, P, S> p)
      const noexcept {
        return _calc(N - 1, 0, t, p, std::make_index_sequence<N>());
    }

private:
    static constexpr auto _coef(
      const int m,
      const int i,
      const Parameter t) noexcept {
        using std::pow;
        return binomial(m, i) * pow(t, i) * pow(1 - t, m - i);
    }

    static constexpr auto _calc(const int, const int, const Parameter) noexcept {
        return Type{0};
    }

    template <typename... P>
    static constexpr auto _calc(
      const int m,
      const int i,
      const Parameter t,
      const Type first,
      const P... rest) noexcept -> Type {
        return first * _coef(m, i, t) + _calc(m, i + 1, t, rest...);
    }

    template <typename P, typename S, std::size_t... I>
    static constexpr auto _calc(
      const int m,
      const int i,
      const Parameter t,
      const memory::basic_span<const Type, P, S> p,
      const std::index_sequence<I...>) noexcept -> Type {
        return _calc(m, i, t, p[I]...);
    }
};
//------------------------------------------------------------------------------
template <typename Parameter, typename... CP>
static constexpr inline auto bezier_point(const Parameter t, const CP... cps)
  -> std::common_type_t<CP...> {
    using bt = bezier_t<std::common_type_t<CP...>, Parameter, sizeof...(CP)>;
    return bt{}(t, cps...);
}
//------------------------------------------------------------------------------
} // namespace eagine::math

#endif // EAGINE_MATH_FUNCTIONS_HPP
