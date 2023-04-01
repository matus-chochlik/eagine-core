/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.types:metaprogramming;

import std;
import :basic;

namespace eagine {

/// @brief Compile-time type list template.
/// @ingroup metaprogramming
/// @see mp_is_empty_v
export template <typename... C>
struct mp_list : std::type_identity<mp_list<C...>> {};
//------------------------------------------------------------------------------
/// @brief Implements the is_empty operation on compile-time type lists.
/// @ingroup metaprogramming
/// @note Do not use directly, use mp_is_empty_v.
export template <typename TL>
struct mp_is_empty;

/// @brief Trait indicating whether a type list is empty.
/// @ingroup metaprogramming
/// @see mp_list
export template <typename TL>
constexpr const auto mp_is_empty_v = mp_is_empty<TL>::value;

export template <>
struct mp_is_empty<mp_list<>> : std::true_type {};

export template <typename... T>
struct mp_is_empty<mp_list<T...>> : std::false_type {};
//------------------------------------------------------------------------------
/// @brief Implements the contains operation on compile-time type lists.
/// @ingroup metaprogramming
/// @note Do not use directly, use mp_contains_v.
export template <typename TL, typename T>
struct mp_contains;

/// @brief Trait indicating whether a type list contains a specified type.
/// @ingroup metaprogramming
/// @see mp_list
/// @see mp_contains_t
export template <typename TL, typename T>
constexpr const auto mp_contains_v = mp_contains<TL, T>::value;

/// @brief Trait indicating whether a type list contains a specified type.
/// @ingroup metaprogramming
/// @see mp_list
/// @see mp_contains_v
export template <typename TL, typename T>
using mp_contains_t = typename mp_contains<TL, T>::type;

export template <typename T>
struct mp_contains<mp_list<>, T> : std::false_type {};

export template <typename H, typename... C, typename T>
struct mp_contains<mp_list<H, C...>, T> : mp_contains<mp_list<C...>, T> {};

export template <typename... C, typename T>
struct mp_contains<mp_list<T, C...>, T> : std::true_type {};
//------------------------------------------------------------------------------
/// @brief Implements the intersection operation on compile-time type lists.
/// @ingroup metaprogramming
/// @note Do not use directly, use mp_intersection_t.
export template <typename TLl, typename TLr, typename TLf>
struct mp_intersection_add;

export template <typename... Tf>
struct mp_intersection_add<mp_list<>, mp_list<>, mp_list<Tf...>>
  : mp_list<Tf...> {};

export template <typename... Tl, typename... Tf>
struct mp_intersection_add<mp_list<Tl...>, mp_list<>, mp_list<Tf...>>
  : mp_list<Tf...> {};

export template <typename... Tr, typename... Tf>
struct mp_intersection_add<mp_list<>, mp_list<Tr...>, mp_list<Tf...>>
  : mp_list<Tf...> {};

export template <typename H, typename... Tl, typename... Tr, typename... Tf>
struct mp_intersection_add<mp_list<H, Tl...>, mp_list<Tr...>, mp_list<Tf...>>
  : std::conditional_t<
      mp_contains<mp_list<Tr...>, H>::value,
      mp_intersection_add<mp_list<Tl...>, mp_list<Tr...>, mp_list<Tf..., H>>,
      mp_intersection_add<mp_list<Tl...>, mp_list<Tr...>, mp_list<Tf...>>> {};

/// @brief Implements the intersection operation on compile-time type lists.
/// @ingroup metaprogramming
/// @note Do not use directly, use mp_intersection_t.
export template <typename TLl, typename TLr>
using mp_intersection = mp_intersection_add<TLl, TLr, mp_list<>>;

/// @brief Trait returning the intersection of types in two type lists.
/// @ingroup metaprogramming
/// @see mp_list
export template <typename TLl, typename TLr>
using mp_intersection_t = typename mp_intersection<TLl, TLr>::type;
//------------------------------------------------------------------------------
/// @brief Implements the union operation on compile-time type lists.
/// @ingroup metaprogramming
/// @note Do not use directly, use mp_union_t.
export template <typename TLl, typename TLr, typename TLf>
struct mp_union_add;

export template <typename... Tf>
struct mp_union_add<mp_list<>, mp_list<>, mp_list<Tf...>> : mp_list<Tf...> {};

export template <typename... Tl, typename... Tf>
struct mp_union_add<mp_list<Tl...>, mp_list<>, mp_list<Tf...>>
  : mp_list<Tf..., Tl...> {};

export template <typename... Tr, typename... Tf>
struct mp_union_add<mp_list<>, mp_list<Tr...>, mp_list<Tf...>>
  : mp_list<Tf..., Tr...> {};

export template <
  typename Hl,
  typename Hr,
  typename... Tl,
  typename... Tr,
  typename... Tf>
struct mp_union_add<mp_list<Hl, Tl...>, mp_list<Hr, Tr...>, mp_list<Tf...>>
  : std::conditional_t<
      mp_contains<mp_list<Tf...>, Hl>::value,
      mp_union_add<mp_list<Tl...>, mp_list<Hr, Tr...>, mp_list<Tf...>>,
      mp_union_add<mp_list<Tl...>, mp_list<Hr, Tr...>, mp_list<Tf..., Hl>>> {};

/// @brief Implements the union operation on compile-time type lists.
/// @ingroup metaprogramming
/// @note Do not use directly, use mp_union_t.
export template <typename TLl, typename TLr>
using mp_union = mp_union_add<TLl, TLr, mp_list<>>;

/// @brief Trait returning the union of types in two type lists.
/// @ingroup metaprogramming
/// @see mp_list
export template <typename TLl, typename TLr>
using mp_union_t = typename mp_union<TLl, TLr>::type;
//------------------------------------------------------------------------------
// mp_string
export template <char... C>
struct mp_string {
    using type = mp_string;

    using value_type = const char[sizeof...(C) + 1];
    static constexpr const char value[sizeof...(C) + 1] = {C..., '\0'};

    constexpr operator std::string_view() const noexcept {
        return {value};
    }
};
//------------------------------------------------------------------------------
// mp_strlen
export template <typename String>
struct mp_strlen;

export template <char... C>
struct mp_strlen<mp_string<C...>> : size_constant<sizeof...(C)> {};
//------------------------------------------------------------------------------
// mp_concat
export template <typename... S>
struct mp_concat;

export template <typename... S>
using mp_concat_t = typename mp_concat<S...>::type;

export template <char... C>
struct mp_concat<mp_string<C...>> : mp_string<C...> {};

export template <char... C1, char... C2, typename... S>
struct mp_concat<mp_string<C1...>, mp_string<C2...>, S...>
  : mp_concat_t<mp_string<C1..., C2...>, S...> {};
//------------------------------------------------------------------------------
// mp_translate
export template <typename S, template <char> class Transf>
struct mp_translate;

export template <typename S, template <char> class Transf>
using mp_translate_t = typename mp_translate<S, Transf>::type;

export template <char... C, template <char> class Transf>
struct mp_translate<mp_string<C...>, Transf>
  : mp_concat_t<typename Transf<C>::type...> {};
//------------------------------------------------------------------------------
// mp_uint_to_string
export template <unsigned I>
struct mp_uint_to_string;

export template <unsigned I>
using mp_uint_to_string_t = typename mp_uint_to_string<I>::type;

export template <>
struct mp_uint_to_string<0U> : mp_string<'0'> {};
export template <>
struct mp_uint_to_string<1U> : mp_string<'1'> {};
export template <>
struct mp_uint_to_string<2U> : mp_string<'2'> {};
export template <>
struct mp_uint_to_string<3U> : mp_string<'3'> {};
export template <>
struct mp_uint_to_string<4U> : mp_string<'4'> {};
export template <>
struct mp_uint_to_string<5U> : mp_string<'5'> {};
export template <>
struct mp_uint_to_string<6U> : mp_string<'6'> {};
export template <>
struct mp_uint_to_string<7U> : mp_string<'7'> {};
export template <>
struct mp_uint_to_string<8U> : mp_string<'8'> {};
export template <>
struct mp_uint_to_string<9U> : mp_string<'9'> {};

export template <unsigned I>
struct mp_uint_to_string
  : mp_concat_t<mp_uint_to_string_t<I / 10>, mp_uint_to_string_t<I % 10>> {};

// mp_int_to_string
export template <int I>
struct mp_int_to_string
  : std::conditional_t<
      (I < 0),
      mp_concat<mp_string<'-'>, mp_uint_to_string_t<unsigned(-I)>>,
      mp_uint_to_string_t<unsigned(I)>> {};

export template <int I>
using mp_int_to_string_t = typename mp_int_to_string<I>::type;
//------------------------------------------------------------------------------
// bits
namespace bits {

// mp_create_string
template <typename S, typename IS>
struct mp_create_string;

template <typename S, std::size_t... I>
struct mp_create_string<S, std::index_sequence<I...>>
  : mp_string<S::mp_str[I]...> {};

// mp_do_make_string
template <typename S, typename A>
struct mp_do_make_string;

template <typename S, std::size_t N>
struct mp_do_make_string<S, const char[N]>
  : mp_create_string<S, std::make_index_sequence<N - 1>> {};

template <typename X>
auto does_have_mp_str_const(X*, decltype(X::mp_str) = X::mp_str)
  -> std::true_type;

auto does_have_mp_str_const(...) -> std::false_type;

} // namespace bits

// has_mp_str_const
export template <typename T>
struct has_mp_str_const
  : decltype(bits::does_have_mp_str_const(static_cast<T*>(nullptr))) {};

export template <typename T>
using has_mp_str_const_t = typename has_mp_str_const<T>::type;
//------------------------------------------------------------------------------
// mp_make_string
export template <typename S>
struct mp_make_string : bits::mp_do_make_string<S, decltype(S::mp_str)> {};

export template <typename S>
using mp_make_string_t = typename mp_make_string<S>::type;
//------------------------------------------------------------------------------
export template <char C>
struct mp_superscript;

export template <char C>
using mp_superscript_t = typename mp_superscript<C>::type;

export template <>
struct mp_superscript<'i'> : mp_string<char(0xE2), char(0x81), char(0xB1)> {};

export template <>
struct mp_superscript<'n'> : mp_string<char(0xE2), char(0x81), char(0xBF)> {};

export template <>
struct mp_superscript<'+'> : mp_string<char(0xE2), char(0x81), char(0xBA)> {};

export template <>
struct mp_superscript<'-'> : mp_string<char(0xE2), char(0x81), char(0xBB)> {};

export template <>
struct mp_superscript<'='> : mp_string<char(0xE2), char(0x81), char(0xBC)> {};

export template <>
struct mp_superscript<'('> : mp_string<char(0xE2), char(0x81), char(0xBD)> {};

export template <>
struct mp_superscript<')'> : mp_string<char(0xE2), char(0x81), char(0xBE)> {};

export template <>
struct mp_superscript<'0'> : mp_string<char(0xE2), char(0x81), char(0xB0)> {};

export template <>
struct mp_superscript<'1'> : mp_string<char(0xC2), char(0xB9)> {};

export template <>
struct mp_superscript<'2'> : mp_string<char(0xC2), char(0xB2)> {};

export template <>
struct mp_superscript<'3'> : mp_string<char(0xC2), char(0xB3)> {};

export template <>
struct mp_superscript<'4'> : mp_string<char(0xE2), char(0x81), char(0xB4)> {};

export template <>
struct mp_superscript<'5'> : mp_string<char(0xE2), char(0x81), char(0xB5)> {};

export template <>
struct mp_superscript<'6'> : mp_string<char(0xE2), char(0x81), char(0xB6)> {};

export template <>
struct mp_superscript<'7'> : mp_string<char(0xE2), char(0x81), char(0xB7)> {};

export template <>
struct mp_superscript<'8'> : mp_string<char(0xE2), char(0x81), char(0xB8)> {};

export template <>
struct mp_superscript<'9'> : mp_string<char(0xE2), char(0x81), char(0xB9)> {};
//------------------------------------------------------------------------------
// mp_subscript
export template <char C>
struct mp_subscript;

export template <char C>
using mp_subscript_t = typename mp_subscript<C>::type;

export template <>
struct mp_subscript<'0'> : mp_string<char(0xE2), char(0x82), char(0x80)> {};

export template <>
struct mp_subscript<'1'> : mp_string<char(0xE2), char(0x82), char(0x81)> {};

export template <>
struct mp_subscript<'2'> : mp_string<char(0xE2), char(0x82), char(0x82)> {};

export template <>
struct mp_subscript<'3'> : mp_string<char(0xE2), char(0x82), char(0x83)> {};

export template <>
struct mp_subscript<'4'> : mp_string<char(0xE2), char(0x82), char(0x84)> {};

export template <>
struct mp_subscript<'5'> : mp_string<char(0xE2), char(0x82), char(0x85)> {};

export template <>
struct mp_subscript<'6'> : mp_string<char(0xE2), char(0x82), char(0x86)> {};

export template <>
struct mp_subscript<'7'> : mp_string<char(0xE2), char(0x82), char(0x87)> {};

export template <>
struct mp_subscript<'8'> : mp_string<char(0xE2), char(0x82), char(0x88)> {};

export template <>
struct mp_subscript<'9'> : mp_string<char(0xE2), char(0x82), char(0x89)> {};

export template <>
struct mp_subscript<'+'> : mp_string<char(0xE2), char(0x82), char(0x8A)> {};

export template <>
struct mp_subscript<'-'> : mp_string<char(0xE2), char(0x82), char(0x8B)> {};
//------------------------------------------------------------------------------
} // namespace eagine
