/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.units.framework:base;

import eagine.core.types;
import <cmath>;
import <type_traits>;

namespace eagine::units {
//------------------------------------------------------------------------------
template <typename X>
struct dimension_of : X::dimension {};

template <>
struct dimension_of<nothing_t> : nothing_t {};

template <typename X>
using dimension_of_t = typename dimension_of<X>::type;
//------------------------------------------------------------------------------
namespace scales {
//------------------------------------------------------------------------------
template <typename X>
struct scale_of : std::type_identity<scale_of<X>> {};

template <typename X>
using scale_of_t = typename scale_of<X>::type;
//------------------------------------------------------------------------------
struct one : std::type_identity<one> {

    template <typename T>
    static constexpr auto to_base(const T v) noexcept {
        return v;
    }

    template <typename T>
    static constexpr auto from_base(const T v) noexcept {
        return v;
    }
};

template <>
struct scale_of<nothing_t> : one {};
//------------------------------------------------------------------------------
template <int I>
struct constant : std::type_identity<constant<I>> {

    template <typename T>
    static constexpr auto to_base(const T v) {
        return v * T(I);
    }

    template <typename T>
    static constexpr auto from_base(const T v) {
        return v / T(I);
    }
};
//------------------------------------------------------------------------------
template <int Num, int Den>
struct rational : std::type_identity<rational<Num, Den>> {

    template <typename T>
    static constexpr auto to_base(const T v) {
        return (v * T(Num)) / T(Den);
    }

    template <typename T>
    static constexpr auto from_base(const T v) {
        return (v * T(Den)) / T(Num);
    }
};
//------------------------------------------------------------------------------
template <int X, int Y>
struct power : std::type_identity<power<X, Y>> {

    template <typename T>
    static constexpr auto to_base(const T v) {
        using std::pow;
        return v * T(pow(X, Y));
    }

    template <typename T>
    static constexpr auto from_base(const T v) {
        using std::pow;
        return v / T(pow(X, Y));
    }
};
//------------------------------------------------------------------------------
template <typename S>
struct inverted : std::type_identity<inverted<S>> {

    template <typename T>
    static constexpr auto to_base(const T v) {
        return S::from_base(v);
    }

    template <typename T>
    static constexpr auto from_base(const T v) {
        return S::to_base(v);
    }
};
//------------------------------------------------------------------------------
template <typename S1, typename S2>
struct multiplied : std::type_identity<multiplied<S1, S2>> {

    template <typename T>
    static constexpr auto to_base(const T v) {
        return S2::to_base(S1::to_base(v));
    }

    template <typename T>
    static constexpr auto from_base(const T v) {
        return S2::from_base(S1::from_base(v));
    }
};
//------------------------------------------------------------------------------
template <typename S1, typename S2>
struct divided : std::type_identity<divided<S1, S2>> {

    template <typename T>
    static constexpr auto to_base(const T v) {
        return S2::from_base(S1::to_base(v));
    }

    template <typename T>
    static constexpr auto from_base(const T v) {
        return S2::to_base(S1::from_base(v));
    }
};
//------------------------------------------------------------------------------
template <typename S1, typename S2>
struct recombined : multiplied<S1, S2> {
    using type = recombined;
};
//------------------------------------------------------------------------------
} // namespace scales
namespace base {
//------------------------------------------------------------------------------
export template <typename Derived>
struct dimension : std::type_identity<Derived> {};

template <typename Dimension, typename Derived>
struct unit;

template <typename Scale, typename Unit>
struct scaled_unit;
//------------------------------------------------------------------------------
template <typename Dimension>
struct dim_num;

template <typename Dim>
struct dim_num<dimension<Dim>> : dim_num<Dim> {};

template <typename Dimension>
const int dim_num_v = dim_num<Dimension>::value;

template <>
struct dim_num<nothing_t> : int_constant<0> {};
//------------------------------------------------------------------------------
} // namespace base
namespace bits {
//------------------------------------------------------------------------------
template <typename X>
struct collapse_tail : std::type_identity<X> {};

template <typename X>
using collapse_tail_t = typename collapse_tail<X>::type;

template <typename Dim, int Pow>
struct dim_pow {
    using dim = Dim;
    using pow = int_constant<Pow>;
};

template <typename Head, typename Tail>
struct dims : std::type_identity<dims<Head, Tail>> {};

using dimless = dims<nothing_t, nothing_t>;

template <>
struct collapse_tail<dims<nothing_t, nothing_t>> : nothing_t {};

template <typename Head, typename Tail>
struct unit_scales : std::type_identity<unit_scales<Head, Tail>> {};

template <>
struct collapse_tail<unit_scales<nothing_t, nothing_t>> : nothing_t {};
//------------------------------------------------------------------------------
template <typename D, typename Dims>
struct pow_of_dim;

template <typename D, typename Dims>
constexpr int pow_of_dim_v = pow_of_dim<D, Dims>::value;

template <typename D>
struct pow_of_dim<D, nothing_t> : int_constant<0> {};

template <typename D>
struct pow_of_dim<D, dimless> : int_constant<0> {};

template <typename D, int P, typename T>
struct pow_of_dim<D, dims<dim_pow<D, P>, T>> : int_constant<P> {};

template <typename D1, typename D2, int P, typename T>
struct pow_of_dim<D1, dims<dim_pow<D2, P>, T>> : pow_of_dim<D1, T> {};

// get_pow_of_dim
template <typename D, typename H, typename T>
static constexpr auto get_pow_of_dim(
  const base::dimension<D>,
  const dims<H, T>) noexcept -> int {
    return pow_of_dim_v<D, dims<H, T>>;
}
//------------------------------------------------------------------------------
template <typename Dims1, typename Dims2>
struct dim_add;

template <typename Dims1, typename Dims2>
using dim_add_t = typename dim_add<Dims1, Dims2>::type;

template <>
struct dim_add<nothing_t, nothing_t> : nothing_t {};

template <typename H, typename T>
struct dim_add<nothing_t, dims<H, T>> : dims<H, T> {};

template <typename H, typename T>
struct dim_add<dims<H, T>, nothing_t> : dims<H, T> {};

template <>
struct dim_add<dimless, dimless> : dimless {};

template <typename H, typename T>
struct dim_add<dims<H, T>, dimless> : dims<H, T> {};

template <typename H, typename T>
struct dim_add<dimless, dims<H, T>> : dims<H, T> {};

template <typename Dim, int Pow1, typename Tail1, int Pow2, typename Tail2>
struct dim_add<dims<dim_pow<Dim, Pow1>, Tail1>, dims<dim_pow<Dim, Pow2>, Tail2>>
  : std::conditional_t<
      (Pow1 + Pow2 == 0),
      dim_add_t<Tail1, Tail2>,
      dims<dim_pow<Dim, Pow1 + Pow2>, collapse_tail_t<dim_add_t<Tail1, Tail2>>>> {
};

template <
  typename Dim1,
  int Pow1,
  typename Tail1,
  typename Dim2,
  int Pow2,
  typename Tail2>
struct dim_add<dims<dim_pow<Dim1, Pow1>, Tail1>, dims<dim_pow<Dim2, Pow2>, Tail2>>
  : std::conditional_t<
      (base::dim_num_v<Dim1> < base::dim_num_v<Dim2>),
      dims<
        dim_pow<Dim1, Pow1>,
        collapse_tail_t<dim_add_t<Tail1, dims<dim_pow<Dim2, Pow2>, Tail2>>>>,
      dims<
        dim_pow<Dim2, Pow2>,
        collapse_tail_t<dim_add_t<dims<dim_pow<Dim1, Pow1>, Tail1>, Tail2>>>> {
};
//------------------------------------------------------------------------------
template <typename Dims1, typename Dims2>
struct dim_sub;

template <typename Dims1, typename Dims2>
using dim_sub_t = typename dim_sub<Dims1, Dims2>::type;

template <>
struct dim_sub<nothing_t, nothing_t> : dimless {};

template <>
struct dim_sub<nothing_t, dimless> : dimless {};

template <typename H, typename T>
struct dim_sub<dims<H, T>, nothing_t> : dims<H, T> {};

template <typename D, int P, typename T>
struct dim_sub<nothing_t, dims<dim_pow<D, P>, T>>
  : dims<dim_pow<D, -P>, collapse_tail_t<dim_sub_t<nothing_t, T>>> {};

template <>
struct dim_sub<dimless, dimless> : dimless {};

template <typename H, typename T>
struct dim_sub<dims<H, T>, dimless> : dims<H, T> {};

template <typename D, int P, typename T>
struct dim_sub<dimless, dims<dim_pow<D, P>, T>>
  : dims<dim_pow<D, -P>, collapse_tail_t<dim_sub_t<nothing_t, T>>> {};

template <typename Dim, int Pow1, typename Tail1, int Pow2, typename Tail2>
struct dim_sub<dims<dim_pow<Dim, Pow1>, Tail1>, dims<dim_pow<Dim, Pow2>, Tail2>>
  : std::conditional_t<
      (Pow1 - Pow2 == 0),
      dim_sub_t<Tail1, Tail2>,
      dims<dim_pow<Dim, Pow1 - Pow2>, collapse_tail_t<dim_sub_t<Tail1, Tail2>>>> {
};

template <
  typename Dim1,
  int Pow1,
  typename Tail1,
  typename Dim2,
  int Pow2,
  typename Tail2>
struct dim_sub<dims<dim_pow<Dim1, Pow1>, Tail1>, dims<dim_pow<Dim2, Pow2>, Tail2>>
  : std::conditional_t<
      (base::dim_num_v<Dim1> < base::dim_num_v<Dim2>),
      dims<
        dim_pow<Dim1, Pow1>,
        collapse_tail_t<dim_sub_t<Tail1, dims<dim_pow<Dim2, Pow2>, Tail2>>>>,
      dims<
        dim_pow<Dim2, -Pow2>,
        collapse_tail_t<dim_sub_t<dims<dim_pow<Dim1, Pow1>, Tail1>, Tail2>>>> {
};
//------------------------------------------------------------------------------
template <typename Dims, typename Dim>
struct get_pow;

template <typename Dim>
struct get_pow<nothing_t, Dim> : int_constant<0> {};

template <typename Dim>
struct get_pow<dimless, Dim> : int_constant<0> {};

template <typename H, typename T, typename Dim>
struct get_pow<dims<H, T>, Dim> : get_pow<T, Dim> {};

template <typename Dim, int Pow, typename T>
struct get_pow<dims<dim_pow<Dim, Pow>, T>, Dim> : int_constant<Pow> {};
//------------------------------------------------------------------------------
template <typename Unit, typename Scale>
struct uni_sca;
//------------------------------------------------------------------------------
template <typename UnitScales, typename Unit, typename Scale>
struct insert;

template <typename UnitScales, typename Unit, typename Scale>
using insert_t = typename insert<UnitScales, Unit, Scale>::type;

template <typename U, typename S>
struct insert<nothing_t, U, S> : unit_scales<uni_sca<U, S>, nothing_t> {};

template <typename U, typename S>
struct insert<unit_scales<nothing_t, nothing_t>, U, S>
  : unit_scales<uni_sca<U, S>, nothing_t> {};

template <typename U, typename SO, typename SN, typename T>
struct insert<unit_scales<uni_sca<U, SO>, T>, U, SN>
  : unit_scales<uni_sca<U, SN>, T> {};

template <typename H, typename T, typename U, typename S>
struct insert<unit_scales<H, T>, U, S> : unit_scales<H, insert_t<T, U, S>> {};
//------------------------------------------------------------------------------
template <typename UnitScales, typename Unit, typename Fallback>
struct get_scale;

template <typename UnitScales, typename Unit, typename Fallback>
using get_scale_t = typename get_scale<UnitScales, Unit, Fallback>::type;

template <typename U, typename Fallback>
struct get_scale<nothing_t, U, Fallback> : Fallback {};

template <typename U, typename Fallback>
struct get_scale<unit_scales<nothing_t, nothing_t>, U, Fallback> : Fallback {};

template <typename U, typename Scale, typename T, typename F>
struct get_scale<unit_scales<uni_sca<U, Scale>, T>, U, F> : Scale {};

template <typename U, typename S1, typename S2, typename T, typename F>
struct get_scale<unit_scales<uni_sca<U, S1>, T>, base::scaled_unit<S2, U>, F>
  : scales::divided<S1, S2> {};

template <typename H, typename T, typename U, typename F>
struct get_scale<unit_scales<H, T>, U, F> : get_scale<T, U, F> {};
//------------------------------------------------------------------------------
template <typename UnitScales, typename BaseDim, typename Fallback>
struct get_dim_unit;

template <typename BD, typename Fallback>
struct get_dim_unit<nothing_t, BD, Fallback> : Fallback {};

template <typename BD, typename Fallback>
struct get_dim_unit<unit_scales<nothing_t, nothing_t>, BD, Fallback>
  : Fallback {};

template <typename U, typename S, typename T, typename BD, typename F>
struct get_dim_unit<unit_scales<uni_sca<U, S>, T>, BD, F>
  : std::conditional_t<
      std::is_same_v<dimension_of_t<U>, BD>,
      base::scaled_unit<S, U>,
      typename get_dim_unit<T, BD, F>::type> {};
//------------------------------------------------------------------------------
template <typename UnitConv1, typename UnitConv2>
struct merge;

template <typename UnitConv1, typename UnitConv2>
using merge_t = typename merge<UnitConv1, UnitConv2>::type;

template <>
struct merge<nothing_t, nothing_t> : unit_scales<nothing_t, nothing_t> {};

template <>
struct merge<unit_scales<nothing_t, nothing_t>, unit_scales<nothing_t, nothing_t>>
  : unit_scales<nothing_t, nothing_t> {};

template <typename H, typename T>
struct merge<unit_scales<H, T>, nothing_t> : unit_scales<H, T> {};

template <typename H, typename T>
struct merge<nothing_t, unit_scales<H, T>> : unit_scales<H, T> {};

template <typename H, typename T>
struct merge<unit_scales<H, T>, unit_scales<nothing_t, nothing_t>>
  : unit_scales<H, T> {};

template <typename H, typename T>
struct merge<unit_scales<nothing_t, nothing_t>, unit_scales<H, T>>
  : unit_scales<H, T> {};

template <typename U, typename S1, typename S2, typename T1, typename T2>
struct merge<unit_scales<uni_sca<U, S1>, T1>, unit_scales<uni_sca<U, S2>, T2>>
  : unit_scales<uni_sca<U, S1>, collapse_tail_t<merge_t<T1, T2>>> {};

template <
  typename U1,
  typename U2,
  typename S1,
  typename S2,
  typename T1,
  typename T2>
struct merge<unit_scales<uni_sca<U1, S1>, T1>, unit_scales<uni_sca<U2, S2>, T2>>
  : std::conditional_t<
      (base::dim_num_v<dimension_of_t<U1>> <
       base::dim_num_v<dimension_of_t<U2>>),
      unit_scales<
        uni_sca<U1, S1>,
        collapse_tail_t<merge_t<T1, unit_scales<uni_sca<U2, S2>, T2>>>>,
      unit_scales<
        uni_sca<U2, S2>,
        collapse_tail_t<merge_t<unit_scales<uni_sca<U1, S1>, T1>, T2>>>> {};
//------------------------------------------------------------------------------
} // namespace bits
//------------------------------------------------------------------------------
export template <typename BaseDim, int Pow>
using dimension = bits::dims<bits::dim_pow<BaseDim, Pow>, nothing_t>;

export template <typename Dims, typename System>
struct unit;

export template <typename Dims, typename Scales, typename System>
struct custom_dim_unit;

template <typename Dims, typename Scales, typename System>
struct scaled_dim_unit_conv;

export template <typename Dims, typename Scales, typename System>
using scaled_dim_unit =
  custom_dim_unit<Dims, scaled_dim_unit_conv<Dims, Scales, System>, System>;
//------------------------------------------------------------------------------
export template <typename X, typename Y>
struct same_dimension : std::false_type {};

export template <typename D, typename S>
struct same_dimension<unit<D, S>, unit<D, S>> : std::true_type {};

export template <typename D, typename US, typename S>
struct same_dimension<unit<D, S>, custom_dim_unit<D, US, S>>
  : std::true_type {};

export template <typename D, typename US, typename S>
struct same_dimension<custom_dim_unit<D, US, S>, unit<D, S>>
  : std::true_type {};

export template <typename D, typename US1, typename US2, typename S>
struct same_dimension<custom_dim_unit<D, US1, S>, custom_dim_unit<D, US2, S>>
  : std::true_type {};

export template <typename X, typename Y>
constexpr bool same_dimension_v = same_dimension<X, Y>::value;
//------------------------------------------------------------------------------
export template <typename U1>
struct lit_result;

export template <typename U1>
using lit_result_t = typename lit_result<U1>::type;

export template <typename U1, typename U2>
struct add_result;

export template <typename U>
struct add_result<U, U> : std::type_identity<U> {};

export template <typename U1, typename U2>
using add_result_t = typename add_result<U1, U2>::type;

export template <typename U1, typename U2>
struct sub_result;

export template <typename U>
struct sub_result<U, U> : std::type_identity<U> {};

export template <typename U1, typename U2>
using sub_result_t = typename sub_result<U1, U2>::type;

export template <typename U1, typename U2>
struct mul_l_operand;

export template <typename U1, typename U2>
using mul_l_operand_t = typename mul_l_operand<U1, U2>::type;

export template <typename U1, typename U2>
struct mul_r_operand;

export template <typename U1, typename U2>
using mul_r_operand_t = typename mul_r_operand<U1, U2>::type;

export template <typename U1, typename U2>
struct mul_result;

export template <typename U1, typename U2>
using mul_result_t = typename mul_result<U1, U2>::type;

export template <typename U1, typename U2>
struct div_l_operand : mul_l_operand<U1, U2> {};

export template <typename U1, typename U2>
using div_l_operand_t = typename div_l_operand<U1, U2>::type;

export template <typename U1, typename U2>
struct div_r_operand : mul_r_operand<U1, U2> {};

export template <typename U1, typename U2>
using div_r_operand_t = typename div_r_operand<U1, U2>::type;

export template <typename U1, typename U2>
struct div_result;

export template <typename U1, typename U2>
using div_result_t = typename div_result<U1, U2>::type;
//------------------------------------------------------------------------------
export template <typename X>
struct is_dimension : std::false_type {};

export template <typename X>
constexpr const bool is_dimension_v = is_dimension<X>::value;

export template <typename H, typename T>
struct is_dimension<bits::dims<H, T>> : std::true_type {};
//------------------------------------------------------------------------------
export template <typename X>
struct is_unit : std::false_type {};

export template <typename D, typename S>
struct is_unit<unit<D, S>> : std::true_type {};

export template <typename D, typename US, typename S>
struct is_unit<custom_dim_unit<D, US, S>> : std::true_type {};

export template <typename X>
constexpr bool is_unit_v = is_unit<X>::value;
//------------------------------------------------------------------------------
export template <typename T1, typename T2>
struct is_convertible : std::false_type {};

export template <typename U>
struct is_convertible<U, U> : std::true_type {};

export template <typename U1, typename U2>
constexpr const bool is_convertible_v = is_convertible<U1, U2>::value;
//------------------------------------------------------------------------------
struct trivial_value_conv {
    template <typename T>
    constexpr auto operator()(const T v) const noexcept -> T {
        return v;
    }
};

export template <typename F, typename T>
struct value_conv;

export template <typename U>
struct value_conv<U, U> : trivial_value_conv {};
//------------------------------------------------------------------------------
export template <typename Item>
struct name_of {
    static constexpr nothing_t mp_str{};
};

export template <typename Item>
struct symbol_of {
    static constexpr nothing_t mp_str{};
};
//------------------------------------------------------------------------------
} // namespace eagine::units
