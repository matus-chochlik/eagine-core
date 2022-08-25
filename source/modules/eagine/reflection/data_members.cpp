/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.reflection:data_members;

import eagine.core.types;
import eagine.core.memory;
import :decl_name;
export import <array>;
import <cstdint>;
import <optional>;
import <tuple>;
import <type_traits>;

namespace eagine {
//------------------------------------------------------------------------------
export template <typename C, typename... M>
constexpr auto make_data_member_mapping(
  std::pair<const string_view, M C::*const>... m) noexcept {
    return std::make_tuple(m...);
}
//------------------------------------------------------------------------------
export template <typename T, typename Selector>
concept mapped_struct = requires(std::type_identity<T> tid, Selector s) {
                            data_member_mapping(tid, s);
                        };

export template <typename T>
concept default_mapped_struct = requires(std::type_identity<T> tid) {
                                    data_member_mapping(tid, default_selector);
                                };
//------------------------------------------------------------------------------
export template <typename F, typename S, identifier_t Id>
constexpr auto data_member_mapping(
  const std::type_identity<std::pair<F, S>>,
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

export template <typename T, identifier_t V>
constexpr auto data_member_tuple(
  const std::type_identity<T> tid,
  const selector<V> sel) noexcept
    requires(mapped_struct<T, selector<V>>)
{
    return data_member_tuple_from_mapping(data_member_mapping(tid, sel));
}
//------------------------------------------------------------------------------
template <typename T, identifier_t V>
constexpr auto _do_map_single_data_member(
  const string_view name,
  const T& ref,
  const selector<V>,
  const std::false_type) noexcept {
    return std::pair<const string_view, const T&>(name, ref);
}
//------------------------------------------------------------------------------
template <typename T, identifier_t V>
constexpr auto _do_map_single_data_member(
  const string_view name,
  T& ref,
  const selector<V>,
  const std::false_type) noexcept {
    return std::pair<const string_view, T&>(name, ref);
}
//------------------------------------------------------------------------------
template <typename T, identifier_t V>
constexpr auto _do_map_single_data_member(
  const string_view name,
  const T& ref,
  const selector<V> select,
  const std::true_type) noexcept {
    return std::make_pair(name, map_data_members(ref, select));
}
//------------------------------------------------------------------------------
template <typename T, identifier_t V>
constexpr auto _do_map_single_data_member(
  const string_view name,
  T& ref,
  const selector<V> select,
  const std::true_type) noexcept {
    return std::make_pair(name, map_data_members(ref, select));
}
//------------------------------------------------------------------------------
template <typename T, identifier_t V>
constexpr auto _map_single_data_member(
  const string_view name,
  const T& ref,
  const selector<V> select) noexcept {
    return _do_map_single_data_member(
      name, ref, select, std::bool_constant<mapped_struct<T, selector<V>>>());
}
//------------------------------------------------------------------------------
template <typename T, identifier_t V>
constexpr auto _map_single_data_member(
  const string_view name,
  T& ref,
  const selector<V> select) noexcept {
    return _do_map_single_data_member(
      name, ref, select, std::bool_constant<mapped_struct<T, selector<V>>>());
}
//------------------------------------------------------------------------------
template <typename T, identifier_t V, typename Mapping, std::size_t... I>
constexpr auto _map_data_members_impl(
  const T& instance,
  const selector<V> select,
  const Mapping& mapping,
  const std::index_sequence<I...>) {
    return std::make_tuple(_map_single_data_member(
      std::get<0>(std::get<I>(mapping)),
      instance.*std::get<1>(std::get<I>(mapping)),
      select)...);
}
//------------------------------------------------------------------------------
template <typename T, identifier_t V, typename Mapping, std::size_t... I>
constexpr auto _map_data_members_impl(
  T& instance,
  const selector<V> select,
  const Mapping& mapping,
  const std::index_sequence<I...>) {
    return std::make_tuple(_map_single_data_member(
      std::get<0>(std::get<I>(mapping)),
      instance.*std::get<1>(std::get<I>(mapping)),
      select)...);
}
//------------------------------------------------------------------------------
template <typename T, identifier_t V, typename C, typename... M>
constexpr auto do_map_data_members(
  T& instance,
  const selector<V> select,
  const std::tuple<std::pair<const string_view, M C::*const>...>& mapping) {
    return _map_data_members_impl(
      instance, select, mapping, std::make_index_sequence<sizeof...(M)>());
}
//------------------------------------------------------------------------------
export template <typename T, identifier_t Id>
constexpr auto map_data_members(
  const T& instance,
  const selector<Id> select) noexcept {
    return do_map_data_members(
      instance,
      select,
      data_member_mapping(std::type_identity<std::remove_cv_t<T>>(), select));
}
//------------------------------------------------------------------------------
export template <typename T, identifier_t Id>
constexpr auto map_data_members(
  T& instance,
  const selector<Id> select) noexcept {
    return do_map_data_members(
      instance,
      select,
      data_member_mapping(std::type_identity<std::remove_cv_t<T>>(), select));
}
//------------------------------------------------------------------------------
export template <typename T>
constexpr auto map_data_members(const T& instance) noexcept {
    return map_data_members(instance, default_selector);
}
//------------------------------------------------------------------------------
export template <typename T>
constexpr auto map_data_members(T& instance) noexcept {
    return map_data_members(instance, default_selector);
}
//------------------------------------------------------------------------------
} // namespace eagine
