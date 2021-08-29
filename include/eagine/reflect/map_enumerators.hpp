/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_REFLECT_MAP_ENUMERATORS_HPP
#define EAGINE_REFLECT_MAP_ENUMERATORS_HPP

#include "../selector.hpp"
#include "config.hpp"
#include "decl_name.hpp"
#include <array>
#include <type_traits>

namespace eagine {
//------------------------------------------------------------------------------
template <typename T>
struct enumerator_and_name {
    constexpr enumerator_and_name(const decl_name n, const T e) noexcept
      : name{n}
      , enumerator{e} {}

    const decl_name name;
    const T enumerator;
};
//------------------------------------------------------------------------------
template <typename T, std::size_t N>
using enumerator_map_type = std::array<enumerator_and_name<T>, N>;
//------------------------------------------------------------------------------
template <typename T, typename Selector>
struct does_have_enumerator_mapping {
private:
    template <
      typename X,
      typename = decltype(enumerator_mapping(type_identity<X>{}, Selector{}))>
    static auto _test(X*) -> std::true_type;
    static auto _test(...) -> std::false_type;

public:
    // NOLINTNEXTLINE(hicpp-vararg)
    using type = decltype(_test(static_cast<std::remove_cv_t<T>*>(nullptr)));
};
//------------------------------------------------------------------------------
template <typename T, typename Selector = default_selector_t>
using has_enumerator_mapping_t =
  typename does_have_enumerator_mapping<T, Selector>::type;

template <typename T, typename Selector = default_selector_t>
constexpr const bool has_enumerator_mapping_v =
  has_enumerator_mapping_t<T, Selector>::value;
//------------------------------------------------------------------------------
#if EAGINE_CXX_REFLECTION
template <typename Enum, typename MetaEnumRange, std::size_t... I>
constexpr auto _do_make_enumerator_mapping(
  const type_identity<Enum>,
  const MetaEnumRange range,
  std::index_sequence<I...>) noexcept {
    // TODO
    return enumerator_map_type<Enum, sizeof...(I)>{
      {std::experimental::meta::name_of(_meta_range_at(range, I)), {}}...};
}
//------------------------------------------------------------------------------
template <typename Enum, typename MetaEnumRange>
constexpr auto _make_enumerator_mapping(
  const type_identity<Enum> tid,
  const MetaEnumRange range) noexcept {
    return _do_make_enumerator_mapping(
      tid, range, std::make_index_sequence<size(range)>{});
}
//------------------------------------------------------------------------------
template <
  typename Enum,
  typename Selector,
  typename = std::enable_if_t<std::is_enum_v<Enum>>>
constexpr auto enumerator_mapping(
  const type_identity<Enum> tid,
  const Selector) noexcept {
    return _make_enumerator_mapping(
      tid,
      std::experimental::meta::members_of(
        ^Enum, std::experimental::meta::is_enumerator));
}
#endif
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_REFLECT_MAP_ENUMERATORS_HPP
