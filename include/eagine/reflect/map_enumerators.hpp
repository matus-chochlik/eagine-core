/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_REFLECT_MAP_ENUMERATORS_HPP
#define EAGINE_REFLECT_MAP_ENUMERATORS_HPP

#include "../config/basic.hpp"
#include "../selector.hpp"
#include "decl_name.hpp"
#include "meta.hpp"
#include <array>
#include <type_traits>

namespace eagine {
//------------------------------------------------------------------------------
template <typename T>
struct name_and_enumerator {
    constexpr name_and_enumerator(const decl_name n, const T e) noexcept
      : name{n}
      , enumerator{e} {}

    const decl_name name;
    const T enumerator;
};

template <typename T>
name_and_enumerator(const decl_name, const T) -> name_and_enumerator<T>;
//------------------------------------------------------------------------------
template <typename T, std::size_t N>
using enumerator_map_type = std::array<const name_and_enumerator<T>, N>;
//------------------------------------------------------------------------------
template <typename T, typename Selector>
struct does_have_enumerator_mapping {
private:
    template <
      typename X,
      typename =
        decltype(enumerator_mapping(std::type_identity<X>{}, Selector{}))>
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
    const auto me = meta::members_of(^Enum, meta::is_enumerator);
    return enumerator_map_type<Enum, sizeof...(I)>{
      {{{immediate_function, meta::name_of(_meta_range_at(me, I))},
        ([:_meta_range_at(me, I):])}...}};
}
//------------------------------------------------------------------------------
template <typename Enum>
static constexpr const auto _reflected_enumerator_mapping =
  _make_enumerator_mapping<Enum>(
    std::make_index_sequence<size(std::experimental::meta::members_of(
      ^Enum,
      std::experimental::meta::is_enumerator))>{});
//------------------------------------------------------------------------------
template <typename Enum, typename Selector>
constexpr auto enumerator_mapping(
  const std::type_identity<Enum>,
  const Selector) noexcept -> const auto&
    requires(std::is_enum_v<Enum>)
{
    return _reflected_enumerator_mapping<Enum>;
}
#endif
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_REFLECT_MAP_ENUMERATORS_HPP
