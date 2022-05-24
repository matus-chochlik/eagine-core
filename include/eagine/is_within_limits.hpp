/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#ifndef EAGINE_IS_WITHIN_LIMITS_HPP
#define EAGINE_IS_WITHIN_LIMITS_HPP

#include "assert.hpp"
#include "diagnostic.hpp"
#include "int_constant.hpp"
#include "valid_if/decl.hpp"
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>

EAGINE_DIAG_PUSH()
#if defined(__clang__)
EAGINE_DIAG_OFF(shorten-64-to-32)
#elif defined(__GNUC__)
EAGINE_DIAG_OFF(sign-compare)
#endif

namespace eagine {
//------------------------------------------------------------------------------
template <typename Dst, typename Src>
struct implicitly_within_limits
  : bool_constant<(
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
        using Tmp = std::make_unsigned_t<Src>;

        return (value < Src(0)) ? false : (Tmp(value) < Dnl::max());
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
template <typename Dst, typename Src>
struct within_limits
  : within_limits_num<
      Dst,
      Src,
      std::is_integral_v<Dst>,
      std::is_integral_v<Src>,
      std::is_signed_v<Dst>,
      std::is_signed_v<Src>> {};
//------------------------------------------------------------------------------
template <typename T>
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
template <typename Dst, typename Src>
static constexpr auto is_within_limits(const Src value) noexcept {
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
template <typename Dst, typename Src>
static constexpr auto limit_cast(Src value) noexcept -> Dst
  requires(std::is_convertible_v<Src, Dst>) {
    if constexpr(std::is_trivial_v<Src> && std::is_trivial_v<Dst>) {
        return EAGINE_CONSTEXPR_ASSERT(
          is_within_limits<Dst>(value), Dst(value));
    } else {
        return EAGINE_CONSTEXPR_ASSERT(
          is_within_limits<Dst>(value), Dst(std::move(value)));
    }
}
//------------------------------------------------------------------------------
/// @brief Casts @p value to a type with the opposite signedness.
/// @ingroup type_utils
/// @see is_within_limits
/// @see convert_if_fits
/// @see limit_cast
template <typename Src>
static constexpr auto signedness_cast(Src value) noexcept {
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
template <typename Dst, typename Src>
static constexpr auto convert_if_fits(Src value) noexcept
  -> optionally_valid<Dst> requires(std::is_convertible_v<Src, Dst>) {

    if(is_within_limits<Dst>(value)) {
        if constexpr(std::is_trivial_v<Src> && std::is_trivial_v<Dst>) {
            return {Dst(value), true};
        } else {
            return {Dst(std::move(value)), true};
        }
    }
    return {};
}
//------------------------------------------------------------------------------
template <std::integral Int>
static constexpr auto are_safe_to_add(Int l, Int r) noexcept -> bool {
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
template <std::integral I>
static constexpr auto safe_add(I i) noexcept {
    return i;
}
//------------------------------------------------------------------------------
template <std::integral L, std::integral R>
static constexpr auto safe_add(L l, R r) noexcept {
    using T = std::common_type_t<L, R>;
    return EAGINE_CONSTEXPR_ASSERT(are_safe_to_add<T>(l, r), l + r);
}
//------------------------------------------------------------------------------
template <std::integral A, std::integral B, std::integral C>
static constexpr auto safe_add(A a, B b, C c) noexcept {
    return safe_add(safe_add(a, b), c);
}
//------------------------------------------------------------------------------
template <std::integral A, std::integral B, std::integral C, std::integral D>
static constexpr auto safe_add(A a, B b, C c, D d) noexcept {
    return safe_add(safe_add(a, b), safe_add(c, d));
}
//------------------------------------------------------------------------------
template <
  std::integral A,
  std::integral B,
  std::integral C,
  std::integral D,
  std::integral E,
  std::integral... P>
static constexpr auto safe_add(A a, B b, C c, D d, E e, P... p) noexcept {
    return safe_add(safe_add(a, b, c, d), safe_add(e, p...));
}
//------------------------------------------------------------------------------
template <std::integral L, std::integral R, std::integral C>
static constexpr auto safe_add_eq(L l, R r, C c) noexcept -> bool {
    using T = std::common_type_t<L, R>;
    return are_safe_to_add<T>(l, r) && (l + r == c);
}
//------------------------------------------------------------------------------
template <std::integral L, std::integral R, std::integral C>
static constexpr auto safe_add_lt(L l, R r, C c) noexcept -> bool {
    using T = std::common_type_t<L, R>;
    return are_safe_to_add<T>(l, r) && (l + r < c);
}
//------------------------------------------------------------------------------
template <std::integral L, std::integral R, std::integral C>
static constexpr auto safe_add_gt(L l, R r, C c) noexcept -> bool {
    using T = std::common_type_t<L, R>;
    return are_safe_to_add<T>(l, r) && (l + r > c);
}
//------------------------------------------------------------------------------
} // namespace eagine

EAGINE_DIAG_POP()

#endif // EAGINE_IS_WITHIN_LIMITS_HPP
