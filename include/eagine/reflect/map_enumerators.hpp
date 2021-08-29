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
using enumerator_map_type = std::array<const enumerator_and_name<T>, N>;
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
template <typename Enum, std::size_t... I>
consteval auto _make_enumerator_mapping(
  const std::index_sequence<I...>) noexcept {
    namespace meta = std::experimental::meta;
    return enumerator_map_type<Enum, sizeof...(I)>{
      {{decl_name{meta::name_of(
          _meta_range_at(meta::members_of(^Enum, meta::is_enumerator), I))},
        ([:_meta_range_at(
             meta::members_of(^Enum, meta::is_enumerator), I):])}...}};
}
//------------------------------------------------------------------------------
template <typename Enum>
static constexpr const auto _reflected_enumerator_mapping =
  _make_enumerator_mapping<Enum>(
    std::make_index_sequence<size(std::experimental::meta::members_of(
      ^Enum,
      std::experimental::meta::is_enumerator))>{});
//------------------------------------------------------------------------------
template <
  typename Enum,
  typename Selector,
  typename = std::enable_if_t<std::is_enum_v<Enum>>>
constexpr auto enumerator_mapping(
  const type_identity<Enum>,
  const Selector) noexcept -> const auto& {
    return _reflected_enumerator_mapping<Enum>;
}
#endif
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_REFLECT_MAP_ENUMERATORS_HPP
