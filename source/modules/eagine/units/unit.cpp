/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.units:unit;

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
export template <typename Dims, typename Conv, typename System>
struct custom_dim_unit {
    using dimension = Dims;
    using conversion = Conv;
    using system = System;
    using type = custom_dim_unit;
};

export template <typename Conv, typename Unit>
struct make_custom_unit;

export template <typename Conv, typename Unit>
using make_custom_unit_t = typename make_custom_unit<Conv, Unit>::type;

export template <typename Conv, typename Dimension, typename System>
struct make_custom_unit<Conv, unit<Dimension, System>>
  : custom_dim_unit<Dimension, Conv, System> {};
//------------------------------------------------------------------------------
export template <typename Dims, typename Scales, typename System>
struct scaled_dim_unit_conv {
    using type = scaled_dim_unit_conv;

    using _impl = bits::_sc_unit_sc_hlp<Scales, System>;
    using _ndp = bits::dim_pow<nothing_t, 0>;

    template <typename T>
    static constexpr auto to_base(const T v) {
        return _impl::_hlp(std::true_type(), v, bits::dims<_ndp, Dims>());
    }

    template <typename T>
    static constexpr auto from_base(const T v) {
        return _impl::_hlp(std::false_type(), v, bits::dims<_ndp, Dims>());
    }
};

// make_scaled_base_dim_unit
export template <typename BaseScaledUnit, typename System>
struct make_scaled_base_dim_unit;

export template <typename BaseScaledUnit, typename System>
using make_scaled_base_dim_unit_t =
  typename make_scaled_base_dim_unit<BaseScaledUnit, System>::type;

export template <typename Scale, typename BaseUnit, typename System>
struct make_scaled_base_dim_unit<base::scaled_unit<Scale, BaseUnit>, System>
  : scaled_dim_unit<
      dimension<dimension_of_t<BaseUnit>, 1>,
      bits::unit_scales<bits::uni_sca<BaseUnit, Scale>, nothing_t>,
      System> {};

template <typename Scale, typename UnitScales>
using add_none_unit_scale_t =
  bits::unit_scales<bits::uni_sca<nothing_t, Scale>, UnitScales>;

// make_scaled_unit
template <typename Scale, typename Unit>
struct make_scaled_unit;

template <typename Scale, typename Unit>
using make_scaled_unit_t = typename make_scaled_unit<Scale, Unit>::type;

template <typename Scale, typename Dimension, typename System>
struct make_scaled_unit<Scale, unit<Dimension, System>>
  : scaled_dim_unit<Dimension, add_none_unit_scale_t<Scale, nothing_t>, System> {
};
//------------------------------------------------------------------------------
export template <typename D, typename C, typename S>
struct is_convertible<custom_dim_unit<D, C, S>, unit<D, S>> : std::true_type {};

export template <typename D, typename C, typename S>
struct is_convertible<unit<D, S>, custom_dim_unit<D, C, S>> : std::true_type {};

export template <typename D, typename C, typename S>
struct is_convertible<custom_dim_unit<D, C, S>, custom_dim_unit<D, C, S>>
  : std::true_type {};

export template <typename D, typename C1, typename C2, typename S>
struct is_convertible<custom_dim_unit<D, C1, S>, custom_dim_unit<D, C2, S>>
  : std::true_type {};
//------------------------------------------------------------------------------
// value conv
export template <typename D, typename S>
struct value_conv<unit<D, S>, unit<D, S>> : trivial_value_conv {};

export template <typename D, typename C, typename S>
struct value_conv<custom_dim_unit<D, C, S>, unit<D, S>> {
    template <typename T>
    constexpr auto operator()(const T v) const {
        return custom_dim_unit<D, C, S>::conversion::to_base(v);
    }
};

export template <typename D, typename C, typename S>
struct value_conv<unit<D, S>, custom_dim_unit<D, C, S>> {
    template <typename T>
    constexpr auto operator()(const T v) const {
        return custom_dim_unit<D, C, S>::conversion::from_base(v);
    }
};

export template <typename D, typename C, typename S>
struct value_conv<custom_dim_unit<D, C, S>, custom_dim_unit<D, C, S>>
  : trivial_value_conv {};

export template <typename D, typename C1, typename C2, typename S>
struct value_conv<custom_dim_unit<D, C1, S>, custom_dim_unit<D, C2, S>> {
    template <typename T>
    constexpr auto operator()(const T v) const {
        return custom_dim_unit<D, C2, S>::conversion::from_base(
          custom_dim_unit<D, C1, S>::conversion::to_base(v));
    }
};

export template <typename D, typename AS, typename US, typename System>
struct value_conv<
  scaled_dim_unit<D, add_none_unit_scale_t<AS, US>, System>,
  unit<D, System>> {
    template <typename T>
    constexpr auto operator()(const T v) const {
        return AS::to_base(v);
    }
};

export template <typename D, typename AS, typename US, typename System>
struct value_conv<
  unit<D, System>,
  scaled_dim_unit<D, add_none_unit_scale_t<AS, US>, System>> {
    template <typename T>
    constexpr auto operator()(const T v) const {
        return AS::from_base(v);
    }
};

export template <typename D, typename AS1, typename AS2, typename US, typename System>
struct value_conv<
  scaled_dim_unit<D, add_none_unit_scale_t<AS1, US>, System>,
  scaled_dim_unit<D, add_none_unit_scale_t<AS2, US>, System>> {
    template <typename T>
    constexpr auto operator()(const T v) const {
        return AS2::from_base(AS1::to_base(v));
    }
};
//------------------------------------------------------------------------------
// lit_result
export template <typename D, typename C, typename S>
struct lit_result<custom_dim_unit<D, C, S>> : custom_dim_unit<D, C, S> {};

export template <typename D, typename S>
struct lit_result<unit<D, S>>
  : scaled_dim_unit<D, bits::unit_scales<nothing_t, nothing_t>, S> {};

export template <typename D1, typename D2, typename US1, typename US2, typename S>
struct mul_l_operand<scaled_dim_unit<D1, US1, S>, scaled_dim_unit<D2, US2, S>>
  : scaled_dim_unit<D1, bits::merge_t<US1, US2>, S> {};

export template <typename D1, typename D2, typename US1, typename US2, typename S>
struct mul_r_operand<scaled_dim_unit<D1, US1, S>, scaled_dim_unit<D2, US2, S>>
  : scaled_dim_unit<D2, bits::merge_t<US1, US2>, S> {};

export template <typename D1, typename D2, typename US1, typename US2, typename S>
struct mul_result<scaled_dim_unit<D1, US1, S>, scaled_dim_unit<D2, US2, S>>
  : scaled_dim_unit<bits::dim_add_t<D1, D2>, bits::merge_t<US1, US2>, S> {};

export template <typename D1, typename D2, typename US1, typename US2, typename S>
struct div_result<scaled_dim_unit<D1, US1, S>, scaled_dim_unit<D2, US2, S>>
  : scaled_dim_unit<bits::dim_sub_t<D1, D2>, bits::merge_t<US1, US2>, S> {};
//------------------------------------------------------------------------------
// add_result
export template <typename D, typename S>
struct add_result<unit<D, S>, unit<D, S>> : unit<D, S> {};

export template <typename D, typename C, typename S>
struct add_result<custom_dim_unit<D, C, S>, unit<D, S>> : unit<D, S> {};

export template <typename D, typename C, typename S>
struct add_result<unit<D, S>, custom_dim_unit<D, C, S>> : unit<D, S> {};

export template <typename D, typename C, typename S>
struct add_result<custom_dim_unit<D, C, S>, custom_dim_unit<D, C, S>>
  : custom_dim_unit<D, C, S> {};

export template <typename D, typename C1, typename C2, typename S>
struct add_result<custom_dim_unit<D, C1, S>, custom_dim_unit<D, C2, S>>
  : custom_dim_unit<D, C1, S> {};

// sub_result
export template <typename D, typename S>
struct sub_result<unit<D, S>, unit<D, S>> : unit<D, S> {};

export template <typename D, typename C, typename S>
struct sub_result<custom_dim_unit<D, C, S>, unit<D, S>> : unit<D, S> {};

export template <typename D, typename C, typename S>
struct sub_result<unit<D, S>, custom_dim_unit<D, C, S>> : unit<D, S> {};

export template <typename D, typename C, typename S>
struct sub_result<custom_dim_unit<D, C, S>, custom_dim_unit<D, C, S>>
  : custom_dim_unit<D, C, S> {};

export template <typename D, typename C1, typename C2, typename S>
struct sub_result<custom_dim_unit<D, C1, S>, custom_dim_unit<D, C2, S>>
  : custom_dim_unit<D, C1, S> {};

// mul_l_operand
export template <typename D1, typename D2, typename S>
struct mul_l_operand<unit<D1, S>, unit<D2, S>> : unit<D1, S> {};

export template <typename D1, typename D2, typename C, typename S>
struct mul_l_operand<custom_dim_unit<D1, C, S>, unit<D2, S>> : unit<D1, S> {};

export template <typename D1, typename D2, typename C, typename S>
struct mul_l_operand<unit<D1, S>, custom_dim_unit<D2, C, S>> : unit<D1, S> {};

export template <typename D1, typename D2, typename C1, typename C2, typename S>
struct mul_l_operand<custom_dim_unit<D1, C1, S>, custom_dim_unit<D2, C2, S>>
  : unit<D1, S> {};

// mul_r_operand
export template <typename D1, typename D2, typename S>
struct mul_r_operand<unit<D1, S>, unit<D2, S>> : unit<D2, S> {};

export template <typename D1, typename D2, typename C, typename S>
struct mul_r_operand<custom_dim_unit<D1, C, S>, unit<D2, S>> : unit<D2, S> {};

export template <typename D1, typename D2, typename C, typename S>
struct mul_r_operand<unit<D1, S>, custom_dim_unit<D2, C, S>> : unit<D2, S> {};

export template <typename D1, typename D2, typename C1, typename C2, typename S>
struct mul_r_operand<custom_dim_unit<D1, C1, S>, custom_dim_unit<D2, C2, S>>
  : unit<D1, S> {};

// mul_result
export template <typename D1, typename D2, typename S>
struct mul_result<unit<D1, S>, unit<D2, S>>
  : unit<bits::dim_add_t<D1, D2>, S> {};

export template <typename D1, typename D2, typename C, typename S>
struct mul_result<custom_dim_unit<D1, C, S>, unit<D2, S>>
  : unit<bits::dim_add_t<D1, D2>, S> {};

export template <typename D1, typename D2, typename C, typename S>
struct mul_result<unit<D1, S>, custom_dim_unit<D2, C, S>>
  : unit<bits::dim_add_t<D1, D2>, S> {};

export template <typename D1, typename D2, typename C1, typename C2, typename S>
struct mul_result<custom_dim_unit<D1, C1, S>, custom_dim_unit<D2, C2, S>>
  : unit<bits::dim_add_t<D1, D2>, S> {};

// div_result
export template <typename D1, typename D2, typename S>
struct div_result<unit<D1, S>, unit<D2, S>>
  : unit<bits::dim_sub_t<D1, D2>, S> {};

export template <typename D1, typename D2, typename C, typename S>
struct div_result<custom_dim_unit<D1, C, S>, unit<D2, S>>
  : unit<bits::dim_sub_t<D1, D2>, S> {};

export template <typename D1, typename D2, typename C, typename S>
struct div_result<unit<D1, S>, custom_dim_unit<D2, C, S>>
  : unit<bits::dim_sub_t<D1, D2>, S> {};

export template <typename D1, typename D2, typename C1, typename C2, typename S>
struct div_result<custom_dim_unit<D1, C1, S>, custom_dim_unit<D2, C2, S>>
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
