/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.math:functions;

import <cmath>;

import :constants;
import <cmath>;
import <concepts>;
import <limits>;
import <optional>;
import <type_traits>;
import <utility>;

namespace eagine::math {
//------------------------------------------------------------------------------
/// @brief Indicates if @p value is a positive integral power of two.
/// @ingroup math
export template <std::integral T>
[[nodiscard]] constexpr auto is_positive_power_of_2(const T value) noexcept
  -> bool {
    using U = std::make_unsigned_t<T>;
    return (value > 0) && ((U(value) & (U(value) - 1)) == 0);
}
//------------------------------------------------------------------------------
/// @brief Returns the greates common divisor of arguments @p l and @p r.
/// @ingroup math
export template <std::integral T>
[[nodiscard]] constexpr auto greatest_common_divisor(
  const T l,
  const T r) noexcept -> T {
    return (r == T(0)) ? l : greatest_common_divisor(r, T(l % r));
}
//------------------------------------------------------------------------------
/// @brief Returns 1 if @p x is non-negative, returns -1 otherwise.
/// @ingroup math
export template <typename T>
[[nodiscard]] constexpr auto signum(const T x) noexcept -> T {
    return (x < 0) ? T(-1) : T(1);
}
//------------------------------------------------------------------------------
/// @brief Returns the minimum value of @p a and @p b.
/// @ingroup math
export template <typename T>
[[nodiscard]] constexpr auto minimum(const T a, const T b) noexcept -> T {
    return a < b ? a : b;
}
//------------------------------------------------------------------------------
/// @brief Returns the minimum value from multiple arguments.
/// @ingroup math
export template <typename T, typename... P>
[[nodiscard]] constexpr auto minimum(
  const T a,
  const T b,
  const T c,
  const P... d) noexcept {
    return minimum(minimum(a, b), c, d...);
}
//------------------------------------------------------------------------------
/// @brief Returns the maximum value of @p a and @p b.
/// @ingroup math
export template <typename T>
[[nodiscard]] constexpr auto maximum(const T a, const T b) noexcept -> T {
    return a > b ? a : b;
}
//------------------------------------------------------------------------------
/// @brief Returns the maximum value from multiple arguments.
/// @ingroup math
export template <typename T, typename... P>
[[nodiscard]] constexpr auto maximum(
  const T a,
  const T b,
  const T c,
  const P... d) noexcept {
    return maximum(maximum(a, b), c, d...);
}
//------------------------------------------------------------------------------
/// @brief Returns @p a divided by @p b if @p b is not zero.
/// @ingroup math
export template <typename T>
[[nodiscard]] constexpr auto ratio(const T a, const T b) noexcept
  -> std::optional<T> {
    if(b > T(0) || (b < T(0))) [[likely]] {
        return {a / b};
    }
    return {};
}
//------------------------------------------------------------------------------
/// @brief Returns the reciprocal of @p x if @p x is not zero.
/// @ingroup math
export template <typename T>
[[nodiscard]] constexpr auto reciprocal(const T x) noexcept
  -> std::optional<T> {
    using std::abs;
    if(abs(x) > std::numeric_limits<T>::epsilon()) [[likely]] {
        return {T(1) / x};
    }
    return {};
}
//------------------------------------------------------------------------------
/// @brief Clamps @p x to be between @p min and @p max.
/// @ingroup math
export template <typename T, typename Min, typename Max>
[[nodiscard]] constexpr auto clamp(
  const T x,
  const Min min,
  const Max max) noexcept {
    return x < T(min) ? T(min) : x > T(max) ? T(max) : x;
}
//------------------------------------------------------------------------------
/// @brief Normalizes @p x to (0, 1), where @p start = 0 and @p end = 1.
/// @ingroup math
export template <typename T, typename S, typename E>
[[nodiscard]] constexpr auto ramp(
  const T x,
  const S start,
  const E end) noexcept {
    return (clamp(x, start, end) - T(start)) / (T(end) - T(start));
}
//------------------------------------------------------------------------------
/// @brief Blends @p v1 and @p v2, using @p alpha as the blending factor.
/// @ingroup math
export template <typename T, typename A>
[[nodiscard]] constexpr auto blend(
  const T v1,
  const T v2,
  const A alpha) noexcept {
    return v1 * T(A(1) - alpha) + v2 * T(alpha);
}
//------------------------------------------------------------------------------
export template <typename Tuple, typename A, std::size_t... I>
constexpr auto do_blend_tuple(
  const std::index_sequence<I...>,
  const Tuple& v1,
  const Tuple& v2,
  const A alpha) noexcept -> Tuple {
    return {(std::get<I>(v1) * (A(1) - alpha) + std::get<I>(v2) * alpha)...};
}

export template <typename... T, typename A>
[[nodiscard]] constexpr auto blend(
  const std::tuple<T...>& v1,
  const std::tuple<T...>& v2,
  const A alpha) noexcept {
    return do_blend_tuple(
      std::make_index_sequence<sizeof...(T)>(), v1, v2, alpha);
}
//------------------------------------------------------------------------------
/// @brief Calculates the inverse logistic (log(x) - log(1 - x)) of @p x.
/// @ingroup math
export template <typename T>
[[nodiscard]] constexpr auto inverse_logistic(const T x) noexcept {
    using std::log;
    return log(x) - log(T(1) - x);
}
//------------------------------------------------------------------------------
/// @brief Calculates the logistic (1 / (1 + exp(-x))) of @p x.
/// @ingroup math
export template <typename T>
[[nodiscard]] constexpr auto logistic(const T x) noexcept {
    using std::exp;
    return T(1) / (T(1) + exp(-x));
}
//------------------------------------------------------------------------------
/// @brief Calculates the sigmoid of @p x. The value @p c controls steepness.
/// @ingroup math
/// @pre 0 <= x <= 1
export template <typename T, typename C>
[[nodiscard]] constexpr auto sigmoid01(const T x, const C c) noexcept {
    return logistic(T(c) * inverse_logistic(x));
}
//------------------------------------------------------------------------------
/// @brief Calculates the default sigmoid of @p x.
/// @ingroup math
/// @pre 0 <= x <= 1
export template <typename T>
[[nodiscard]] constexpr auto sigmoid01(const T x) noexcept {
    return sigmoid01(x, 2);
}
//------------------------------------------------------------------------------
/// @brief Calculates goniometric sigmoid (cos in interval (0, 1)) of @p x.
/// @ingroup math
/// @pre 0 <= x <= 1
export template <typename T>
[[nodiscard]] constexpr auto sine_sigmoid01(const T x) noexcept -> T {
    using std::cos;
    return T((T(1) - cos(x * T(pi))) / T(2));
}
//------------------------------------------------------------------------------
/// @brief Calculates sine of @p x, mapped to interval (0, 1).
/// @ingroup math
/// @pre 0 <= x <= 1
export template <typename T>
[[nodiscard]] constexpr auto sine_wave01(const T x) noexcept -> T {
    using std::sin;
    return T((sin(T(2) * T(pi) * x) + T(1)) / T(2));
}
//------------------------------------------------------------------------------
/// @brief Calculates cosine of @p x, mapped to interval (0, 1).
/// @ingroup math
/// @pre 0 <= x <= 1
export template <typename T>
[[nodiscard]] constexpr auto cosine_wave01(const T x) noexcept -> T {
    using std::cos;
    return T((cos(T(2) * T(pi) * x) + T(1)) / T(2));
}
//------------------------------------------------------------------------------
/// @brief Calculates floating-point modulo of @p x in intervals of @p u.
/// @ingroup math
export template <typename T, typename U = T>
[[nodiscard]] constexpr auto saw(const T x, const U u = T(1)) noexcept -> T {
    using std::fmod;
    return T(fmod(x, T(u)));
}
//------------------------------------------------------------------------------
/// @brief Calculates factorial of @p n.
/// @ingroup math
export template <std::integral T>
[[nodiscard]] constexpr auto factorial(const T n) noexcept -> T {
    return n > 0 ? n * factorial(n - 1) : 1;
}
//------------------------------------------------------------------------------
/// @brief Calculates binomial coefficient of @p n over @p k.
/// @ingroup math
export template <std::integral T>
[[nodiscard]] constexpr auto binomial(const T n, const T k) noexcept -> T {
    return ((n >= 0) && (k >= 0) && (k <= n))
             ? (factorial(n) / (factorial(k) * factorial(n - k)))
             : 0;
}
//------------------------------------------------------------------------------
/// @brief Linear interpolation between @p a and @p b.
/// @ingroup math
/// @see lerp
export template <typename T, typename C>
[[nodiscard]] constexpr auto interpolate_linear(
  const T& a,
  const T& b,
  const C coef) noexcept {
    return ((C(1) - coef) * a + coef * b);
}
//------------------------------------------------------------------------------
/// @brief Same as interpolate_linear.
/// @ingroup math
/// @see interpolate_linear
export template <typename T, typename C>
[[nodiscard]] constexpr auto lerp(const T& a, const T& b, const C coef) noexcept {
    return interpolate_linear(a, b, coef);
}
//------------------------------------------------------------------------------
/// @brief Linear interpolation with @p coef transformed by sine_sigmoid01.
/// @ingroup math
/// @see interpolate_linear
/// @see sine_sigmoid01
export template <typename T, typename C>
[[nodiscard]] constexpr auto smooth_lerp(
  const T& a,
  const T& b,
  const C coef) noexcept {
    return lerp(a, b, sine_sigmoid01(coef));
}
//------------------------------------------------------------------------------
/// @brief Smooth interpolation between negative @p a and @p a.
/// @ingroup math
/// @see smooth_lerp
export template <typename T, typename C>
[[nodiscard]] constexpr auto smooth_oscillate(
  const T& a,
  const C coef) noexcept {
    return smooth_lerp(-a, a, coef);
}
//------------------------------------------------------------------------------
} // namespace eagine::math
