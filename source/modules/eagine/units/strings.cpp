/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.units:strings;

import std;
import eagine.core.types;
import eagine.core.memory;
import :base;
import :dimension;
import :unit;

namespace eagine::units {
//------------------------------------------------------------------------------
export template <typename MpStr, int Pow>
struct dim_pow_superscript
  : mp_concat<MpStr, mp_translate_t<mp_int_to_string_t<Pow>, mp_superscript>> {
};
//------------------------------------------------------------------------------
export template <typename MpStr>
struct dim_pow_superscript<MpStr, 0> : mp_string<> {};
//------------------------------------------------------------------------------
export template <typename MpStr>
struct dim_pow_superscript<MpStr, 1> : MpStr {};
//------------------------------------------------------------------------------
export template <typename MpStr, int Pow>
using dim_pow_superscript_t = typename dim_pow_superscript<MpStr, Pow>::type;
//------------------------------------------------------------------------------
export template <typename MpStrL, typename MpStrR>
struct chain_term_strings
  : mp_concat_t<MpStrL, mp_string<char(0xC3), char(0x97)>, MpStrR> {};
//------------------------------------------------------------------------------
export template <typename MpStrL>
struct chain_term_strings<MpStrL, mp_string<>> : MpStrL {};
//------------------------------------------------------------------------------
export template <typename MpStrL, typename MpStrR>
using chain_term_strings_t = typename chain_term_strings<MpStrL, MpStrR>::type;
//------------------------------------------------------------------------------
// generic unit system name
//------------------------------------------------------------------------------
export template <template <typename> class MpStrWrap, typename X, typename MpStr>
struct compose_str;
//------------------------------------------------------------------------------
export template <template <typename> class MpStrWrap, typename X>
using compose_str_t =
  typename compose_str<MpStrWrap, X, decltype(MpStrWrap<X>::mp_str)>::type;
//------------------------------------------------------------------------------
export template <template <typename> class MpStrWrap, typename X>
using compose_str_n = typename compose_str<MpStrWrap, X, nothing_t>::type;
//------------------------------------------------------------------------------
export template <template <typename> class MpStrWrap, typename X>
struct compose_str<MpStrWrap, X, const nothing_t>
  : compose_str_n<MpStrWrap, X> {};
//------------------------------------------------------------------------------
export template <template <typename> class MpStrWrap, typename X, std::size_t N>
struct compose_str<MpStrWrap, X, const char[N]>
  : mp_make_string_t<MpStrWrap<X>> {};
//------------------------------------------------------------------------------
export template <template <typename> class MpStrWrap>
struct compose_str<MpStrWrap, nothing_t, nothing_t> : mp_string<> {};
//------------------------------------------------------------------------------
export template <template <typename> class MpStrWrap, typename BaseDim, int Pow>
struct compose_str<MpStrWrap, bits::dim_pow<BaseDim, Pow>, nothing_t>
  : dim_pow_superscript_t<mp_make_string_t<MpStrWrap<BaseDim>>, Pow> {};
//------------------------------------------------------------------------------
export template <template <typename> class MpStrWrap, typename H, typename T>
struct compose_str<MpStrWrap, bits::dims<H, T>, nothing_t>
  : chain_term_strings<compose_str_t<MpStrWrap, H>, compose_str_t<MpStrWrap, T>> {
};
//------------------------------------------------------------------------------
export template <template <typename> class MpStrWrap, typename Sys>
struct compose_str<MpStrWrap, unit<nothing_t, Sys>, nothing_t> : mp_string<> {};
//------------------------------------------------------------------------------
export template <
  template <typename>
  class MpStrWrap,
  typename Dim,
  int Pow,
  typename Sys>
struct compose_str<MpStrWrap, unit<bits::dim_pow<Dim, Pow>, Sys>, nothing_t>
  : dim_pow_superscript_t<
      compose_str_t<MpStrWrap, typename Sys::template base_unit<Dim>::type>,
      Pow> {};
//------------------------------------------------------------------------------
export template <
  template <typename>
  class MpStrWrap,
  typename H,
  typename T,
  typename Sys>
struct compose_str<MpStrWrap, unit<bits::dims<H, T>, Sys>, nothing_t>
  : chain_term_strings<
      compose_str_t<MpStrWrap, unit<H, Sys>>,
      compose_str_t<MpStrWrap, unit<T, Sys>>> {};

//------------------------------------------------------------------------------
export template <template <typename> class MpStrWrap, typename Scale, typename Unit>
struct compose_str<MpStrWrap, base::scaled_unit<Scale, Unit>, nothing_t>
  : mp_concat<compose_str_t<MpStrWrap, Scale>, compose_str_t<MpStrWrap, Unit>> {
};
//------------------------------------------------------------------------------
export template <typename X>
constexpr auto get_symbol(const X) noexcept -> string_view {
    return compose_str_t<symbol_of, X>::value;
}
//------------------------------------------------------------------------------
export template <typename X>
constexpr auto get_symbol_form(const X) noexcept -> string_view {
    return compose_str_n<symbol_of, X>::value;
}
//------------------------------------------------------------------------------
export template <typename X>
constexpr auto get_name(const X) noexcept -> string_view {
    return compose_str_t<name_of, X>::value;
}
//------------------------------------------------------------------------------
export template <typename X>
constexpr auto get_name_form(const X) noexcept -> string_view {
    return compose_str_n<name_of, X>::value;
}
//------------------------------------------------------------------------------
} // namespace eagine::units

