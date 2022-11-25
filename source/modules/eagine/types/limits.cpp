/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.types:limits;

import :basic;
import <cstdint>;
import <concepts>;
import <limits>;
import <type_traits>;
import <utility>;
export import <optional>;

namespace eagine {
//------------------------------------------------------------------------------
template <typename Dst, typename Src>
struct implicitly_within_limits
  : std::bool_constant<(
      ((std::is_integral_v<Dst> && std::is_integral_v<Src>) ||
       (std::is_floating_point_v<Dst> && std::is_floating_point_v<Src>)) &&
      (std::is_signed_v<Dst> ==
       std::is_signed_v<Src>)&&(sizeof(Dst) >= sizeof(Src)))> {};

template <typename Dst>
struct implicitly_within_limits<Dst, bool> : std::is_integral<Dst> {};

template <>
struct implicitly_within_limits<float, std::int16_t> : std::true_type {};

template <>
struct implicitly_within_limits<float, std::int32_t> : std::true_type {};

template <>
struct implicitly_within_limits<double, std::int16_t> : std::true_type {};

template <>
struct implicitly_within_limits<double, std::int32_t> : std::true_type {};

template <>
struct implicitly_within_limits<double, std::int64_t> : std::true_type {};
//------------------------------------------------------------------------------
template <
  typename Dst,
  typename Src,
  bool DIsInt,
  bool SIsInt,
  bool DIsSig,
  bool SIsSig>
struct within_limits_num {
    static constexpr auto check(const Src) noexcept {
        return implicitly_within_limits<Dst, Src>::value;
    }
};
//------------------------------------------------------------------------------
template <typename Dst, typename Src, bool IsInt, bool IsSig>
struct within_limits_num<Dst, Src, IsInt, IsInt, IsSig, IsSig> {
    static constexpr auto check(const Src value) noexcept {
        using dnl = std::numeric_limits<Dst>;

        return (dnl::min() <= value) && (value <= dnl::max());
    }
};
//------------------------------------------------------------------------------
template <typename Dst, typename Src, bool IsInt>
struct within_limits_num<Dst, Src, IsInt, IsInt, false, true> {
    static constexpr auto check(const Src value) noexcept {
        using Dnl = std::numeric_limits<Dst>;

        if constexpr(IsInt) {
            using Tmp = std::make_unsigned_t<Src>;
            return (value < Src(0)) ? false : (Tmp(value) < Dnl::max());
        } else {
            return implicitly_within_limits<Dst, Src>::value;
        }
    }
};
//------------------------------------------------------------------------------
template <typename Dst, typename Src, bool IsInt>
struct within_limits_num<Dst, Src, IsInt, IsInt, true, false> {
    static constexpr auto check(const Src value) noexcept {
        using dnl = std::numeric_limits<Dst>;

        return (value < dnl::max());
    }
};
//------------------------------------------------------------------------------
export template <typename Dst, typename Src>
struct within_limits
  : within_limits_num<
      Dst,
      Src,
      std::is_integral_v<Dst>,
      std::is_integral_v<Src>,
      std::is_signed_v<Dst>,
      std::is_signed_v<Src>> {};
//------------------------------------------------------------------------------
export template <typename T>
struct within_limits<T, T> {
    static constexpr auto check(const T&) noexcept {
        return true;
    }
};
//------------------------------------------------------------------------------
/// @brief Indicates if @p value fits into the specified Dst type.
/// @ingroup type_utils
/// @see limit_cast
/// @see convert_if_fits
///
/// This function tests if the specified argument would fit into another type.
/// For example if a value stored in 64-bit integer can be converted into
/// a 16-bit integer without overflow.
export template <typename Dst, typename Src>
constexpr auto is_within_limits(const Src value) noexcept {
    return implicitly_within_limits<Dst, Src>::value ||
           within_limits<Dst, Src>::check(value);
}
//------------------------------------------------------------------------------
/// @brief Casts @p value to Dst type if the value fits in that type.
/// @ingroup type_utils
/// @see is_within_limits
/// @see convert_if_fits
/// @see signedness_cast
/// @pre is_within_limits<Dst>(value)
export template <typename Dst, typename Src>
constexpr auto limit_cast(Src value) noexcept -> Dst
    requires(std::is_convertible_v<Src, Dst>)
{
    assert(is_within_limits<Dst>(value));
    if constexpr(std::is_trivial_v<Src> && std::is_trivial_v<Dst>) {
        return Dst(value);
    } else {
        return Dst(std::move(value));
    }
}
//------------------------------------------------------------------------------
/// @brief Casts @p value to a type with the opposite signedness.
/// @ingroup type_utils
/// @see is_within_limits
/// @see convert_if_fits
/// @see limit_cast
export template <typename Src>
constexpr auto signedness_cast(Src value) noexcept {
    return limit_cast<std::conditional_t<
      std::is_signed_v<Src>,
      std::make_unsigned_t<Src>,
      std::make_signed_t<Src>>>(value);
}
//------------------------------------------------------------------------------
/// @brief Optionally converts @p value to Dst type if the value fits in that type.
/// @ingroup type_utils
/// @see is_within_limits
/// @see limit_cast
/// @see assign_if_fits
export template <typename Dst, typename Src>
constexpr auto convert_if_fits(Src value) noexcept -> std::optional<Dst>
    requires(std::is_convertible_v<Src, Dst>)
{
    if(is_within_limits<Dst>(value)) {
        if constexpr(std::is_trivial_v<Src> && std::is_trivial_v<Dst>) {
            return {Dst(value)};
        } else {
            return {Dst(std::move(value))};
        }
    }
    return {};
}

/// @brief Optionally converts and assigns src value to dst value.
/// @ingroup type_utils
/// @see is_within_limits
/// @see limit_cast
/// @see convert_if_fits
export template <typename Src, typename Dst>
constexpr auto assign_if_fits(const Src& src, Dst& dst) noexcept -> bool
    requires(
      std::is_convertible_v<Src, Dst> || std::is_constructible_v<Dst, Src>)
{
    if(is_within_limits<Dst>(src)) {
        if constexpr(std::is_trivial_v<Src> && std::is_trivial_v<Dst>) {
            dst = Dst(src);
        } else {
            dst = Dst(std::move(src));
        }
        return true;
    }
    return false;
}

/// @brief Concept for Dst types for which there is assign_if_fits conversion.
/// @ingroup type_utils
/// @see assign_if_fits
/// @see limit_cast
export template <typename Dst, typename Src>
concept assignable_if_fits_from = requires(const Src src, Dst dst) {
    { assign_if_fits(src, dst) } -> std::convertible_to<bool>;
};

export template <typename Src, typename DstR, typename DstP>
constexpr auto assign_if_fits(
  const Src& src,
  std::chrono::duration<DstR, DstP>& dst) noexcept -> bool
    requires(
      std::is_convertible_v<Src, DstR> || std::is_constructible_v<DstR, Src>)
{
    if(is_within_limits<DstR>(src)) {
        if constexpr(std::is_trivial_v<Src> && std::is_trivial_v<DstR>) {
            dst = std::chrono::duration<DstR, DstP>{DstR(src)};
        } else {
            dst = std::chrono::duration<DstR, DstP>{DstR(std::move(src))};
        }
        return true;
    }
    return false;
}

export template <typename Src, typename Dst>
constexpr auto assign_if_fits(const Src& src, std::optional<Dst>& dst) noexcept
  -> bool
    requires(
      std::is_convertible_v<Src, Dst> || std::is_constructible_v<Dst, Src>)
{
    Dst temp{};
    if(assign_if_fits(src, temp)) {
        dst = std::move(temp);
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
/// @brief Converts argument to span size type.
/// @ingroup type_utils
export template <std::integral T>
constexpr auto span_size(const T v) noexcept {
    return limit_cast<span_size_t>(v);
}

/// @brief Converts argument to std size type.
/// @ingroup type_utils
export template <std::integral T>
constexpr auto std_size(const T v) noexcept {
    return limit_cast<std::size_t>(v);
}

/// @brief Returns the byte alignment of type T as span_size_t.
/// @ingroup type_utils
export template <typename T>
constexpr auto span_align_of(const std::type_identity<T> = {}) noexcept {
    return span_size(alignof(T));
}

/// @brief Returns the byte alignment of type max_align_t as span_size_t.
/// @ingroup type_utils
export constexpr auto max_span_align() noexcept {
    return span_align_of<std::max_align_t>();
}

/// @brief Returns the byte size of type T as span_size_t.
/// @ingroup type_utils
export template <typename T>
constexpr auto span_size_of(const std::type_identity<T> = {}) noexcept {
    return span_size(sizeof(T));
}

/// @brief Returns the total byte size of n elements of type T as span_size_t.
/// @ingroup type_utils
export template <typename T, typename S>
constexpr auto span_size_of(
  const S n,
  const std::type_identity<T> = {}) noexcept {
    return span_size(sizeof(T)) * span_size(n);
}
//------------------------------------------------------------------------------
export template <std::integral Int>
constexpr auto are_safe_to_add(Int l, Int r) noexcept -> bool {
    if constexpr(std::is_signed_v<Int>) {
        if((l < 0) != (r < 0)) {
            return true;
        }
        using std::abs;
        l = abs(l);
        r = abs(r);
    }
    if(std::numeric_limits<Int>::max() - l > r) [[likely]] {
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
export template <std::integral I>
constexpr auto safe_add(I i) noexcept {
    return i;
}
//------------------------------------------------------------------------------
export template <std::integral L, std::integral R>
constexpr auto safe_add(L l, R r) noexcept {
    using T = std::common_type_t<L, R>;
    assert(are_safe_to_add<T>(l, r));
    return l + r;
}
//------------------------------------------------------------------------------
export template <std::integral A, std::integral B, std::integral C>
constexpr auto safe_add(A a, B b, C c) noexcept {
    return safe_add(safe_add(a, b), c);
}
//------------------------------------------------------------------------------
export template <std::integral A, std::integral B, std::integral C, std::integral D>
constexpr auto safe_add(A a, B b, C c, D d) noexcept {
    return safe_add(safe_add(a, b), safe_add(c, d));
}
//------------------------------------------------------------------------------
export template <
  std::integral A,
  std::integral B,
  std::integral C,
  std::integral D,
  std::integral E,
  std::integral... P>
constexpr auto safe_add(A a, B b, C c, D d, E e, P... p) noexcept {
    return safe_add(safe_add(a, b, c, d), safe_add(e, p...));
}
//------------------------------------------------------------------------------
export template <std::integral L, std::integral R, std::integral C>
constexpr auto safe_add_eq(L l, R r, C c) noexcept -> bool {
    using T = std::common_type_t<L, R>;
    return are_safe_to_add<T>(l, r) && (l + r == c);
}
//------------------------------------------------------------------------------
export template <std::integral L, std::integral R, std::integral C>
constexpr auto safe_add_lt(L l, R r, C c) noexcept -> bool {
    using T = std::common_type_t<L, R>;
    return are_safe_to_add<T>(l, r) && (l + r < c);
}
//------------------------------------------------------------------------------
export template <std::integral L, std::integral R, std::integral C>
constexpr auto safe_add_gt(L l, R r, C c) noexcept -> bool {
    using T = std::common_type_t<L, R>;
    return are_safe_to_add<T>(l, r) && (l + r > c);
}
//------------------------------------------------------------------------------
export template <typename... Params, typename... Args>
constexpr auto args_within_limits_of(const Args&... args) noexcept -> bool
    requires(sizeof...(Params) == sizeof...(Args))
{
    return (... && is_within_limits<std::decay_t<Params>>(args));
}
//------------------------------------------------------------------------------
export template <typename RV, typename... Params, typename... Args>
constexpr auto args_within_limits(
  RV (*)(Params...),
  const Args&... args) noexcept -> bool {
    return args_within_limits_of<Params...>(args...);
}
//------------------------------------------------------------------------------
export template <typename RV, typename Cls, typename... Params, typename... Args>
constexpr auto args_within_limits(
  RV (Cls::*)(Params...),
  const Args&... args) noexcept -> bool {
    return args_within_limits_of<Params...>(args...);
}
//------------------------------------------------------------------------------
export template <typename RV, typename Cls, typename... Params, typename... Args>
constexpr auto args_within_limits(
  RV (Cls::*)(Params...) noexcept,
  const Args&... args) noexcept -> bool {
    return args_within_limits_of<Params...>(args...);
}
//------------------------------------------------------------------------------
export template <typename RV, typename Cls, typename... Params, typename... Args>
constexpr auto args_within_limits(
  RV (Cls::*)(Params...) const,
  const Args&... args) noexcept -> bool {
    return args_within_limits_of<Params...>(args...);
}
//------------------------------------------------------------------------------
export template <typename RV, typename Cls, typename... Params, typename... Args>
constexpr auto args_within_limits(
  RV (Cls::*)(Params...) const noexcept,
  const Args&... args) noexcept -> bool {
    return args_within_limits_of<Params...>(args...);
}
//------------------------------------------------------------------------------
} // namespace eagine
