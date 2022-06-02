/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.concepts:type_traits;

import :utils;
import <type_traits>;

namespace eagine {

// all_are_same
template <typename T, typename... P>
using all_are_same = std::conjunction<std::true_type, std::is_same<T, P>...>;

export template <typename T, typename... P>
using all_are_same_t = type_t<all_are_same<T, P...>>;

export template <typename T, typename... P>
constexpr bool all_are_same_v = all_are_same<T, P...>::value;

// all_are_derived_from
template <typename T, typename... P>
using all_are_derived_from =
  std::conjunction<std::true_type, std::is_base_of<T, P>...>;

export template <typename T, typename... P>
using all_are_derived_from_t = type_t<all_are_derived_from<T, P...>>;

export template <typename T, typename... P>
constexpr bool all_are_derived_from_v = all_are_derived_from<T, P...>::value;

// all_are_convertible_to
template <typename T, typename... P>
using all_are_convertible_to =
  std::conjunction<std::true_type, std::is_convertible<P, T>...>;

export template <typename T, typename... P>
using all_are_convertible_to_t = type_t<all_are_convertible_to<T, P...>>;

export template <typename T, typename... P>
constexpr bool all_are_convertible_to_v =
  all_are_convertible_to<T, P...>::value;

} // namespace eagine

