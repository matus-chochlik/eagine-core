/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.reflection:data_members;

import std;
import eagine.core.types;
import eagine.core.memory;
import :decl_name;

namespace eagine {
//------------------------------------------------------------------------------
export template <typename C, typename... M>
constexpr auto make_data_member_mapping(
  std::pair<const string_view, M C::*const>... m) noexcept {
    return std::make_tuple(m...);
}
//------------------------------------------------------------------------------
export template <typename T>
struct data_member_traits;

export template <typename T>
concept mapped_struct = requires(data_member_traits<T> dmt) { dmt.mapping(); };
//------------------------------------------------------------------------------
export template <typename F, typename S>
struct data_member_traits<std::pair<F, S>> {
    static constexpr auto mapping() noexcept {
        using P = std::pair<F, S>;
        return make_data_member_mapping<P, F, S>(
          {"first", &P::first}, {"second", &P::second});
    }
};
//------------------------------------------------------------------------------
template <typename C, typename... M>
constexpr auto data_member_tuple_from_mapping(
  const std::tuple<std::pair<const string_view, M C::*const>...>&) noexcept {
    return std::tuple<std::remove_cv_t<std::remove_reference_t<M>>...>{};
}

export template <mapped_struct T>
constexpr auto data_member_tuple(const std::type_identity<T> tid) noexcept {
    return data_member_tuple_from_mapping(data_member_traits<T>{}.mapping(tid));
}
//------------------------------------------------------------------------------
template <typename T>
constexpr auto _do_map_single_data_member(
  const string_view name,
  const T& ref,
  const std::false_type) noexcept {
    return std::pair<const string_view, const T&>(name, ref);
}
//------------------------------------------------------------------------------
template <typename T>
constexpr auto _do_map_single_data_member(
  const string_view name,
  T& ref,
  const std::false_type) noexcept {
    return std::pair<const string_view, T&>(name, ref);
}
//------------------------------------------------------------------------------
template <typename T>
constexpr auto _do_map_single_data_member(
  const string_view name,
  const T& ref,
  const std::true_type) noexcept {
    using R = decltype(map_data_members(ref));
    return std::pair<const string_view, R>(name, map_data_members(ref));
}
//------------------------------------------------------------------------------
template <typename T>
constexpr auto _do_map_single_data_member(
  const string_view name,
  T& ref,
  const std::true_type) noexcept {
    using R = decltype(map_data_members(ref));
    return std::pair<const string_view, R>(name, map_data_members(ref));
}
//------------------------------------------------------------------------------
template <typename T>
constexpr auto _map_single_data_member(
  const string_view name,
  const T& ref) noexcept {
    return _do_map_single_data_member(
      name, ref, std::bool_constant<mapped_struct<T>>());
}
//------------------------------------------------------------------------------
template <typename T>
constexpr auto _map_single_data_member(const string_view name, T& ref) noexcept {
    return _do_map_single_data_member(
      name, ref, std::bool_constant<mapped_struct<T>>());
}
//------------------------------------------------------------------------------
template <typename T, typename Mapping, std::size_t... I>
constexpr auto _map_data_members_impl(
  const T& instance,
  const Mapping& mapping,
  const std::index_sequence<I...>) {
    return std::make_tuple(_map_single_data_member(
      std::get<0>(std::get<I>(mapping)),
      instance.*std::get<1>(std::get<I>(mapping)))...);
}
//------------------------------------------------------------------------------
template <typename T, typename Mapping, std::size_t... I>
constexpr auto _map_data_members_impl(
  T& instance,
  const Mapping& mapping,
  const std::index_sequence<I...>) {
    return std::make_tuple(_map_single_data_member(
      std::get<0>(std::get<I>(mapping)),
      instance.*std::get<1>(std::get<I>(mapping)))...);
}
//------------------------------------------------------------------------------
template <typename T, typename C, typename... M>
constexpr auto do_map_data_members(
  T& instance,
  const std::tuple<std::pair<const string_view, M C::*const>...>& mapping) {
    return _map_data_members_impl(
      instance, mapping, std::make_index_sequence<sizeof...(M)>());
}
//------------------------------------------------------------------------------
export template <mapped_struct T>
[[nodiscard]] constexpr auto map_data_members(const T& instance) noexcept {
    return do_map_data_members(
      instance, data_member_traits<std::remove_cv_t<T>>{}.mapping());
}
//------------------------------------------------------------------------------
export template <mapped_struct T>
[[nodiscard]] constexpr auto map_data_members(T& instance) noexcept {
    return do_map_data_members(
      instance, data_member_traits<std::remove_cv_t<T>>{}.mapping());
}
//------------------------------------------------------------------------------
export template <typename... M>
constexpr auto data_member_count(
  const std::tuple<std::pair<const string_view, M>...>&) noexcept
  -> span_size_t {
    return span_size(sizeof...(M));
}
//------------------------------------------------------------------------------
} // namespace eagine
