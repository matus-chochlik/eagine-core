/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_REFLECT_DATA_MEMBERS_HPP
#define EAGINE_REFLECT_DATA_MEMBERS_HPP

#include "../selector.hpp"
#include "map_data_members.hpp"

namespace eagine {
//------------------------------------------------------------------------------
template <typename F, typename S, identifier_t Id>
constexpr auto data_member_mapping(
  const type_identity<std::pair<F, S>>,
  const selector<Id>) noexcept {
    using P = std::pair<F, S>;
    return make_data_member_mapping<P, F, S>(
      {"first", &P::first}, {"second", &P::second});
}
//------------------------------------------------------------------------------
template <typename C, typename... M>
constexpr auto data_member_tuple_from_mapping(
  const std::tuple<std::pair<const string_view, M C::*const>...>&) noexcept {
    return std::tuple<std::remove_cv_t<std::remove_reference_t<M>>...>{};
}

template <
  typename T,
  typename Selector,
  typename = std::enable_if_t<has_data_member_mapping_v<T, Selector>>>
constexpr auto data_member_tuple(
  const type_identity<T> tid,
  const Selector sel) noexcept {
    return data_member_tuple_from_mapping(data_member_mapping(tid, sel));
}
//------------------------------------------------------------------------------
template <typename T, typename Selector>
constexpr auto _do_map_single_data_member(
  const string_view name,
  const T& ref,
  const Selector,
  const std::false_type) noexcept {
    return std::pair<const string_view, const T&>(name, ref);
}
//------------------------------------------------------------------------------
template <typename T, typename Selector>
constexpr auto _do_map_single_data_member(
  const string_view name,
  T& ref,
  const Selector,
  const std::false_type) noexcept {
    return std::pair<const string_view, T&>(name, ref);
}
//------------------------------------------------------------------------------
template <typename T, typename Selector>
constexpr auto _do_map_single_data_member(
  const string_view name,
  const T& ref,
  const Selector select,
  const std::true_type) noexcept {
    return std::make_pair(name, map_data_members(ref, select));
}
//------------------------------------------------------------------------------
template <typename T, typename Selector>
constexpr auto _do_map_single_data_member(
  const string_view name,
  T& ref,
  const Selector select,
  const std::true_type) noexcept {
    return std::make_pair(name, map_data_members(ref, select));
}
//------------------------------------------------------------------------------
template <typename T, typename Selector>
constexpr auto _map_single_data_member(
  const string_view name,
  const T& ref,
  const Selector select) noexcept {
    return _do_map_single_data_member(
      name, ref, select, has_data_member_mapping_t<T, Selector>());
}
//------------------------------------------------------------------------------
template <typename T, typename Selector>
constexpr auto _map_single_data_member(
  const string_view name,
  T& ref,
  const Selector select) noexcept {
    return _do_map_single_data_member(
      name, ref, select, has_data_member_mapping_t<T, Selector>());
}
//------------------------------------------------------------------------------
template <typename T, typename Selector, typename Mapping, std::size_t... I>
constexpr auto _map_data_members_impl(
  const T& instance,
  const Selector select,
  const Mapping& mapping,
  const std::index_sequence<I...>) {
    return std::make_tuple(_map_single_data_member(
      std::get<0>(std::get<I>(mapping)),
      instance.*std::get<1>(std::get<I>(mapping)),
      select)...);
}
//------------------------------------------------------------------------------
template <typename T, typename Selector, typename Mapping, std::size_t... I>
constexpr auto _map_data_members_impl(
  T& instance,
  const Selector select,
  const Mapping& mapping,
  const std::index_sequence<I...>) {
    return std::make_tuple(_map_single_data_member(
      std::get<0>(std::get<I>(mapping)),
      instance.*std::get<1>(std::get<I>(mapping)),
      select)...);
}
//------------------------------------------------------------------------------
template <typename T, typename Selector, typename C, typename... M>
constexpr auto do_map_data_members(
  T& instance,
  const Selector select,
  const std::tuple<std::pair<const string_view, M C::*const>...>& mapping) {
    return _map_data_members_impl(
      instance, select, mapping, std::make_index_sequence<sizeof...(M)>());
}
//------------------------------------------------------------------------------
template <typename T, identifier_t Id>
constexpr auto map_data_members(
  const T& instance,
  const selector<Id> select) noexcept {
    return do_map_data_members(
      instance,
      select,
      data_member_mapping(type_identity<std::remove_cv_t<T>>(), select));
}
//------------------------------------------------------------------------------
template <typename T, identifier_t Id>
constexpr auto map_data_members(
  T& instance,
  const selector<Id> select) noexcept {
    return do_map_data_members(
      instance,
      select,
      data_member_mapping(type_identity<std::remove_cv_t<T>>(), select));
}
//------------------------------------------------------------------------------
template <typename T>
constexpr auto map_data_members(const T& instance) noexcept {
    return map_data_members(instance, default_selector);
}
//------------------------------------------------------------------------------
template <typename T>
constexpr auto map_data_members(T& instance) noexcept {
    return map_data_members(instance, default_selector);
}
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_REFLECT_DATA_MEMBERS_HPP
