/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.units:base;

import eagine.core.types;
import <cmath>;
import <type_traits>;

namespace eagine::units {
//------------------------------------------------------------------------------
export template <typename>
struct system_id;
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
export template <typename X>
struct scale_of : std::type_identity<scale_of<X>> {};

export template <typename X>
using scale_of_t = typename scale_of<X>::type;
//------------------------------------------------------------------------------
export struct one : std::type_identity<one> {

    template <typename T>
    static constexpr auto to_base(const T v) noexcept {
        return v;
    }

    template <typename T>
    static constexpr auto from_base(const T v) noexcept {
        return v;
    }
};

export template <>
struct scale_of<nothing_t> : one {};
//------------------------------------------------------------------------------
export template <int I>
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
export struct pi {
    using type = pi;

    static constexpr auto _pi() noexcept {
#ifdef M_PI
        return M_PI;
#else
        return 3.14159265358979323846;
#endif
    }

    template <typename T>
    static constexpr auto to_base(const T v) noexcept {
        return v * T(_pi());
    }

    template <typename T>
    static constexpr auto from_base(const T v) noexcept {
        return v / T(_pi());
    }
};
//------------------------------------------------------------------------------
export template <int Num, int Den>
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
export template <int X, int Y>
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
export template <typename S>
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
export template <typename S1, typename S2>
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
export template <typename S1, typename S2>
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
export template <typename S1, typename S2>
struct recombined : multiplied<S1, S2> {
    using type = recombined;
};
//------------------------------------------------------------------------------
// nano
export using nano = power<1000, -3>;
// micro
export using micro = power<1000, -2>;
// milli
export using milli = power<1000, -1>;
// centi
export using centi = power<10, -2>;
// deci
export using deci = power<10, -1>;
// deca
export using deca = power<10, 1>;
// hecto
export using hecto = power<10, 2>;
// kilo
export using kilo = power<1000, 1>;
// mega
export using mega = power<1000, 2>;
// giga
export using giga = power<1000, 3>;
// tera
export using tera = power<1000, 4>;
// kibi
export using kibi = power<1024, 1>;
// mebi
export using mebi = power<1024, 2>;
// gibi
export using gibi = power<1024, 3>;
// tebi
export using tebi = power<1024, 4>;
//------------------------------------------------------------------------------
} // namespace scales
namespace base {
//------------------------------------------------------------------------------
export template <typename Derived>
struct dimension : std::type_identity<Derived> {};

export template <typename Dimension, typename Derived>
struct unit : std::type_identity<Derived> {
    using dimension = Dimension;
    using scale = scales::one;
};

export template <typename Scale, typename Unit>
struct scaled_unit : std::type_identity<scaled_unit<Scale, Unit>> {
    using dimension = dimension_of_t<Unit>;
    using scale = Scale;
};
//------------------------------------------------------------------------------
template <typename Dimension>
struct dim_num;

template <typename Dim>
struct dim_num<dimension<Dim>> : dim_num<Dim> {};

template <typename Dimension>
const int dim_num_v = dim_num<Dimension>::value;
//------------------------------------------------------------------------------
struct angle : dimension<angle> {};
struct solid_angle : dimension<solid_angle> {};

struct length : dimension<length> {};
struct mass : dimension<mass> {};
struct time : dimension<time> {};

struct temperature : dimension<temperature> {};
struct electric_current : dimension<electric_current> {};
struct number_of_cycles : dimension<number_of_cycles> {};
struct number_of_decays : dimension<number_of_decays> {};
struct luminous_intensity : dimension<luminous_intensity> {};
struct amount_of_substance : dimension<amount_of_substance> {};
struct amount_of_information : dimension<amount_of_information> {};

template <typename Func>
auto for_each_dim(Func func) -> Func {
    func(angle());
    func(solid_angle());
    func(length());
    func(mass());
    func(time());
    func(temperature());
    func(electric_current());
    func(number_of_cycles());
    func(number_of_decays());
    func(luminous_intensity());
    func(amount_of_substance());
    func(amount_of_information());

    return func;
}

template <>
struct dim_num<nothing_t> : int_constant<0> {};

template <>
struct dim_num<angle> : int_constant<1> {};
template <>
struct dim_num<solid_angle> : int_constant<2> {};
template <>
struct dim_num<mass> : int_constant<3> {};
template <>
struct dim_num<length> : int_constant<4> {};
template <>
struct dim_num<time> : int_constant<5> {};

template <>
struct dim_num<temperature> : int_constant<6> {};
template <>
struct dim_num<electric_current> : int_constant<7> {};
template <>
struct dim_num<number_of_cycles> : int_constant<8> {};
template <>
struct dim_num<number_of_decays> : int_constant<9> {};
template <>
struct dim_num<luminous_intensity> : int_constant<10> {};
template <>
struct dim_num<amount_of_substance> : int_constant<11> {};
template <>
struct dim_num<amount_of_information> : int_constant<12> {};

// get_number
template <typename Derived>
static constexpr auto get_number(const dimension<Derived>) noexcept -> int {
    return dim_num_v<Derived>;
}
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

export template <typename Head, typename Tail>
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
export template <typename Unit, typename Scale>
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
// _sc_unit_sc_hlp
template <typename Scales, typename System>
struct _sc_unit_sc_hlp {

    template <typename T, typename SV>
    static constexpr auto _pow(const T v, const SV, const int_constant<0>) {
        return v;
    }

    template <typename T, typename S, int E>
    static constexpr auto _pow(const T v, const S s, const int_constant<E>) {
        return _pow(
          (E > 0) ? S::to_base(v) : S::from_base(v),
          s,
          int_constant<E + ((E > 0) ? -1 : 1)>());
    }

    template <typename Dir, typename T>
    static constexpr auto _hlp(const Dir, const T v) noexcept -> T {
        return v;
    }

    template <typename Dir, typename T>
    static constexpr auto _hlp(const Dir d, const T v, const nothing_t) noexcept
      -> T {
        return _hlp(d, v);
    }

    template <typename Dir, typename T>
    static constexpr auto _hlp(const Dir d, const T v, const dimless) noexcept
      -> T {
        return _hlp(d, v);
    }

    template <typename Dir, typename T, typename D, int E>
    static constexpr auto _hlp2(
      const Dir,
      const T v,
      const dim_pow<D, E>) noexcept {
        using SBU = typename System ::template base_unit<D>::type;
        using BS = scales::scale_of_t<SBU>;

        return _pow(
          v,
          get_scale_t<Scales, SBU, BS>(),
          int_constant<(Dir::value ? E : -E)>());
    }

    template <typename Dir, typename T, typename D, int P, typename Dims>
    static constexpr auto _hlp(
      const Dir dir,
      const T v,
      const dims<dim_pow<D, P>, Dims>) noexcept {
        return _hlp(dir, _hlp2(dir, v, dim_pow<D, P>()), Dims());
    }
};
//------------------------------------------------------------------------------
} // namespace bits
//------------------------------------------------------------------------------
export template <typename BaseDim, int Pow>
using dimension = bits::dims<bits::dim_pow<BaseDim, Pow>, nothing_t>;

export template <typename Dims, typename System>
struct unit;

export template <typename Dims, typename Scales, typename System>
struct custom_dim_unit;

export template <typename Dims, typename Scales, typename System>
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

export template <>
struct name_of<base::angle> {
    static constexpr const char mp_str[] = "angle";
};

export template <>
struct name_of<base::solid_angle> {
    static constexpr const char mp_str[] = "solid angle";
};

export template <>
struct name_of<base::mass> {
    static constexpr const char mp_str[] = "mass";
};

export template <>
struct name_of<base::length> {
    static constexpr const char mp_str[] = "length";
};

export template <>
struct name_of<base::time> {
    static constexpr const char mp_str[] = "time";
};

export template <>
struct name_of<base::temperature> {
    static constexpr const char mp_str[] = "temperature";
};

export template <>
struct name_of<base::electric_current> {
    static constexpr const char mp_str[] = "electric current";
};

export template <>
struct name_of<base::number_of_cycles> {
    static constexpr const char mp_str[] = "number of cycles";
};

export template <>
struct name_of<base::number_of_decays> {
    static constexpr const char mp_str[] = "number of decays";
};

export template <>
struct name_of<base::luminous_intensity> {
    static constexpr const char mp_str[] = "luminous intensity";
};

export template <>
struct name_of<base::amount_of_substance> {
    static constexpr const char mp_str[] = "amount of substance";
};

export template <>
struct name_of<base::amount_of_information> {
    static constexpr const char mp_str[] = "amount of information";
};
//------------------------------------------------------------------------------
export template <typename Item>
struct symbol_of {
    static constexpr nothing_t mp_str{};
};
//------------------------------------------------------------------------------
export template <>
struct name_of<scales::one> {
    static constexpr const char mp_str[] = "";
};
export template <>
struct symbol_of<scales::one> {
    static constexpr const char mp_str[] = "";
};
//------------------------------------------------------------------------------
export template <>
struct name_of<scales::nano> {
    static constexpr const char mp_str[] = "nano";
};
export template <>
struct symbol_of<scales::nano> {
    static constexpr const char mp_str[] = "n";
};
//------------------------------------------------------------------------------
export template <>
struct name_of<scales::micro> {
    static constexpr const char mp_str[] = "micro";
};
export template <>
struct symbol_of<scales::micro> {
    static constexpr const char mp_str[3] = {char(0xCE), char(0xBC), '\0'};
};
//------------------------------------------------------------------------------
template <>
struct name_of<scales::milli> {
    static constexpr const char mp_str[] = "milli";
};
template <>
struct symbol_of<scales::milli> {
    static constexpr const char mp_str[] = "m";
};
//------------------------------------------------------------------------------
template <>
struct name_of<scales::centi> {
    static constexpr const char mp_str[] = "centi";
};
template <>
struct symbol_of<scales::centi> {
    static constexpr const char mp_str[] = "c";
};
//------------------------------------------------------------------------------
template <>
struct name_of<scales::deci> {
    static constexpr const char mp_str[] = "deci";
};
template <>
struct symbol_of<scales::deci> {
    static constexpr const char mp_str[] = "d";
};
//------------------------------------------------------------------------------
template <>
struct name_of<scales::deca> {
    static constexpr const char mp_str[] = "deca";
};
template <>
struct symbol_of<scales::deca> {
    static constexpr const char mp_str[] = "dc";
};
//------------------------------------------------------------------------------
template <>
struct name_of<scales::hecto> {
    static constexpr const char mp_str[] = "hecto";
};
template <>
struct symbol_of<scales::hecto> {
    static constexpr const char mp_str[] = "h";
};
//------------------------------------------------------------------------------
template <>
struct name_of<scales::kilo> {
    static constexpr const char mp_str[] = "kilo";
};
template <>
struct symbol_of<scales::kilo> {
    static constexpr const char mp_str[] = "k";
};
//------------------------------------------------------------------------------
template <>
struct name_of<scales::mega> {
    static constexpr const char mp_str[] = "mega";
};
template <>
struct symbol_of<scales::mega> {
    static constexpr const char mp_str[] = "M";
};
//------------------------------------------------------------------------------
template <>
struct name_of<scales::giga> {
    static constexpr const char mp_str[] = "giga";
};
template <>
struct symbol_of<scales::giga> {
    static constexpr const char mp_str[] = "G";
};
//------------------------------------------------------------------------------
template <>
struct name_of<scales::tera> {
    static constexpr const char mp_str[] = "tera";
};
template <>
struct symbol_of<scales::tera> {
    static constexpr const char mp_str[] = "T";
};
//------------------------------------------------------------------------------
template <>
struct name_of<scales::kibi> {
    static constexpr const char mp_str[] = "kibi";
};
template <>
struct symbol_of<scales::kibi> {
    static constexpr const char mp_str[] = "Ki";
};
//------------------------------------------------------------------------------
template <>
struct name_of<scales::mebi> {
    static constexpr const char mp_str[] = "mebi";
};
template <>
struct symbol_of<scales::mebi> {
    static constexpr const char mp_str[] = "Mi";
};
//------------------------------------------------------------------------------
template <>
struct name_of<scales::gibi> {
    static constexpr const char mp_str[] = "gibi";
};
template <>
struct symbol_of<scales::gibi> {
    static constexpr const char mp_str[] = "Gi";
};
//------------------------------------------------------------------------------
template <>
struct name_of<scales::tebi> {
    static constexpr const char mp_str[] = "tebi";
};
template <>
struct symbol_of<scales::tebi> {
    static constexpr const char mp_str[] = "Ti";
};
//------------------------------------------------------------------------------
template <>
struct name_of<scales::pi> {
    static constexpr const char mp_str[3] = {char(0xCF), char(0x80), '\0'};
};
template <>
struct symbol_of<scales::pi> {
    static constexpr const char mp_str[] = {char(0xCF), char(0x80), '\0'};
};
//------------------------------------------------------------------------------
} // namespace eagine::units
