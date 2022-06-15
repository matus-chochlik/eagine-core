/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.types:metaprogramming;

import <type_traits>;

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
/// @brief Implements the union operation on compile-time type lists.
/// @ingroup metaprogramming
/// @note Do not use directly, use mp_union_t.
export template <typename TL1, typename TL2, typename TL3>
struct mp_union_add;

export template <typename... T3>
struct mp_union_add<mp_list<>, mp_list<>, mp_list<T3...>> : mp_list<T3...> {};

export template <typename... T1, typename... T3>
struct mp_union_add<mp_list<T1...>, mp_list<>, mp_list<T3...>>
  : mp_list<T3...> {};

export template <typename... T2, typename... T3>
struct mp_union_add<mp_list<>, mp_list<T2...>, mp_list<T3...>>
  : mp_list<T3...> {};

export template <typename H, typename... T1, typename... T2, typename... T3>
struct mp_union_add<mp_list<H, T1...>, mp_list<T2...>, mp_list<T3...>>
  : std::conditional_t<
      mp_contains<mp_list<T2...>, H>::value,
      mp_union_add<mp_list<T1...>, mp_list<T2...>, mp_list<T3..., H>>,
      mp_union_add<mp_list<T1...>, mp_list<T2...>, mp_list<T3...>>> {};

/// @brief Implements the union operation on compile-time type lists.
/// @ingroup metaprogramming
/// @note Do not use directly, use mp_union_t.
export template <typename TL1, typename TL2>
using mp_union = mp_union_add<TL1, TL2, mp_list<>>;

/// @brief Trait returning the union of types in two type lists.
/// @ingroup metaprogramming
/// @see mp_list
export template <typename TL1, typename TL2>
using mp_union_t = typename mp_union<TL1, TL2>::type;
//------------------------------------------------------------------------------
} // namespace eagine
