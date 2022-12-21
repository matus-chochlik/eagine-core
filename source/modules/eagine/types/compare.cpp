/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.types:compare;

import <limits>;
import <type_traits>;
import <utility>;

namespace eagine {
//------------------------------------------------------------------------------
template <typename L, typename R>
struct equal_cmp_any {
    static constexpr auto check(const L& l, const R& r) noexcept {
        return l == r;
    }
};
//------------------------------------------------------------------------------
template <typename L, typename R, bool LSign, bool RSign>
struct equal_cmp_int;
//------------------------------------------------------------------------------
template <typename L, typename R, bool Sign>
struct equal_cmp_int<L, R, Sign, Sign> {
    static constexpr auto check(const L l, const R r) noexcept {
        return l == r;
    }
};
//------------------------------------------------------------------------------
template <typename L, typename R>
struct equal_cmp_int<L, R, true, false> {
    static constexpr auto check(const L l, const R r) noexcept {
        using Tmp = std::make_unsigned_t<L>;
        return (l < 0) ? false : Tmp(l) == r;
    }
};
//------------------------------------------------------------------------------
template <typename L, typename R>
struct equal_cmp_int<L, R, false, true> : equal_cmp_int<R, L, true, false> {};
//------------------------------------------------------------------------------
template <typename L, typename R>
using equal_cmp_pick = std::conditional_t<
  std::is_integral_v<L> and std::is_integral_v<R>,
  equal_cmp_int<L, R, std::is_signed_v<L>, std::is_signed_v<R>>,
  equal_cmp_any<L, R>>;
//------------------------------------------------------------------------------
export template <typename L, typename R>
struct equal_cmp : equal_cmp_pick<L, R> {};
//------------------------------------------------------------------------------
export template <>
struct equal_cmp<float, float> {
    static constexpr auto check(const float l, const float r) noexcept {
        return not((l - r) < 0.F or (l - r) > 0.F);
    }
};
//------------------------------------------------------------------------------
export template <>
struct equal_cmp<double, double> {
    static constexpr auto check(const double l, const double r) noexcept {
        return not((l - r) < 0.0 or (l - r) > 0.0);
    }
};
//------------------------------------------------------------------------------
template <typename L, typename R>
struct equal_cmp_obj : equal_cmp<L, R> {
    constexpr equal_cmp_obj(L l, R r) noexcept
      : _l{std::move(l)}
      , _r{std::move(r)} {}

    constexpr explicit inline operator bool() const noexcept {
        return equal_cmp<L, R>::check(_l, _r);
    }

    L _l{};
    R _r{};
};
//------------------------------------------------------------------------------
export template <typename T>
struct cmp_decay_to : std::type_identity<T> {};

export template <typename T>
using cmp_decay_to_t = typename cmp_decay_to<T>::type;
//------------------------------------------------------------------------------
export template <typename L, typename R>
constexpr auto are_equal(const L& l, const R& r) noexcept {
    return equal_cmp<cmp_decay_to_t<L>, cmp_decay_to_t<R>>::check(l, r);
}
//------------------------------------------------------------------------------
} // namespace eagine
