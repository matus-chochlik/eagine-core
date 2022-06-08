/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.units.framework:unit;

import eagine.core.types;
import :base;
import <type_traits>;
import <utility>;

namespace eagine::units {
//------------------------------------------------------------------------------
export template <typename Dims, typename System>
struct unit {
    using dimension = Dims;
    using system = System;
    using scale = scales::one;
    using type = unit;
};

export template <typename X>
auto get_dimension(X) noexcept {
    return dimension_of_t<X>{};
}
//------------------------------------------------------------------------------
// value conv
export template <typename D, typename S>
struct value_conv<unit<D, S>, unit<D, S>> : trivial_value_conv {};

// add_result
export template <typename D, typename S>
struct add_result<unit<D, S>, unit<D, S>> : unit<D, S> {};

// sub_result
export template <typename D, typename S>
struct sub_result<unit<D, S>, unit<D, S>> : unit<D, S> {};

// mul_l_operand
export template <typename D1, typename D2, typename S>
struct mul_l_operand<unit<D1, S>, unit<D2, S>> : unit<D1, S> {};

// mul_r_operand
export template <typename D1, typename D2, typename S>
struct mul_r_operand<unit<D1, S>, unit<D2, S>> : unit<D2, S> {};

// mul_result
export template <typename D1, typename D2, typename S>
struct mul_result<unit<D1, S>, unit<D2, S>>
  : unit<bits::dim_add_t<D1, D2>, S> {};

// div_result
export template <typename D1, typename D2, typename S>
struct div_result<unit<D1, S>, unit<D2, S>>
  : unit<bits::dim_sub_t<D1, D2>, S> {};

// quotation
export template <typename U>
constexpr auto operator!(U) noexcept -> lit_result_t<U>
    requires(is_unit_v<U>)
{
    return {};
}

// addition
export template <typename U1, typename U2>
constexpr auto operator+(U1, U2) noexcept -> add_result_t<U1, U2>
    requires(is_unit_v<U1> && is_unit_v<U2>)
{
    return {};
}

// subtraction
export template <typename U1, typename U2>
constexpr auto operator-(U1, U2) noexcept -> sub_result_t<U1, U2>
    requires(is_unit_v<U1> && is_unit_v<U2>)
{
    return {};
}

// multiplication
export template <typename U1, typename U2>
constexpr auto operator*(U1, U2) noexcept -> mul_result_t<U1, U2>
    requires(is_unit_v<U1> && is_unit_v<U2>)
{
    return {};
}

// division
export template <typename U1, typename U2>
constexpr auto operator/(U1, U2) noexcept -> div_result_t<U1, U2>
    requires(is_unit_v<U1> && is_unit_v<U2>)
{
    return {};
}
//------------------------------------------------------------------------------
} // namespace eagine::units
