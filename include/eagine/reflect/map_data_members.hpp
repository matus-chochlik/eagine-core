/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_REFLECT_MAP_DATA_MEMBERS_HPP
#define EAGINE_REFLECT_MAP_DATA_MEMBERS_HPP

#include "../config/basic.hpp"
#include "../selector.hpp"
#include "../string_span.hpp"
#include "decl_name.hpp"
#include "meta.hpp"
#include <tuple>
#include <type_traits>
#include <utility>

namespace eagine {
//------------------------------------------------------------------------------
template <typename C, typename... M>
constexpr auto make_data_member_mapping(
  std::pair<const string_view, M C::*const>... m) noexcept {
    return std::make_tuple(m...);
}
//------------------------------------------------------------------------------
template <typename T, typename Selector>
struct does_have_data_member_mapping {
private:
    template <
      typename X,
      typename =
        decltype(data_member_mapping(std::type_identity<X>{}, Selector{}))>
    static auto _test(X*) -> std::true_type;
    static auto _test(...) -> std::false_type;

public:
    // NOLINTNEXTLINE(hicpp-vararg)
    using type = decltype(_test(static_cast<std::remove_cv_t<T>*>(nullptr)));
};
//------------------------------------------------------------------------------
template <typename T, typename Selector = default_selector_t>
using has_data_member_mapping_t =
  typename does_have_data_member_mapping<T, Selector>::type;

template <typename T, typename Selector = default_selector_t>
constexpr const bool has_data_member_mapping_v =
  has_data_member_mapping_t<T, Selector>::value;
//------------------------------------------------------------------------------
#if EAGINE_CXX_REFLECTION
template <typename Struct>
struct reflect_data_members_of : std::false_type {};

template <typename Struct, typename... M, std::size_t... I>
consteval auto _make_data_member_mapping(
  const std::index_sequence<I...>) noexcept {
    namespace meta = std::experimental::meta;
    const auto mdm = meta::members_of(^Struct, meta::is_data_member);
    return make_data_member_mapping<
      Struct,
      typename[:meta::type_of(_meta_range_at(mdm, I)):]...>(
      {{immediate_function, meta::name_of(_meta_range_at(mdm, I))},
       &([:_meta_range_at(mdm, I):])}...); // currently crashes the compiler
}
//------------------------------------------------------------------------------
template <typename Struct>
static constexpr const auto _reflected_data_member_mapping =
  _make_data_member_mapping<Struct>(
    std::make_index_sequence<size(std::experimental::meta::members_of(
      ^Struct,
      std::experimental::meta::is_data_member))>{});
//------------------------------------------------------------------------------
template <typename Struct, typename Selector>
constexpr auto data_member_mapping(
  const std::type_identity<Struct>,
  const Selector) noexcept -> const auto&
    requires(reflect_data_members_of<Struct>::value)
{
    return _reflected_data_member_mapping<Struct>;
}
#endif
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_REFLECT_MAP_DATA_MEMBERS_HPP
