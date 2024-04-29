/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.runtime:traverse;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.reflection;

namespace eagine {
//------------------------------------------------------------------------------
// max_mapped_identifier_length
//------------------------------------------------------------------------------
export template <typename T>
    requires(not mapped_struct<T>)
constexpr auto _get_max_identifier_length(T& obj) noexcept -> span_size_t {
    return 0;
}
//------------------------------------------------------------------------------
export template <typename M>
constexpr auto _get_single_max_identifier_length(
  const std::pair<const string_view, M>& m) noexcept -> span_size_t;
//------------------------------------------------------------------------------
export template <typename Map, std::size_t... I>
constexpr auto _get_max_identifier_length_idx(
  const Map& members,
  std::index_sequence<I...>) noexcept -> span_size_t {
    return std::max(
      {_get_single_max_identifier_length(std::get<I>(members))...});
}
//------------------------------------------------------------------------------
export template <typename... M>
constexpr auto _get_max_identifier_length_map(
  const std::tuple<std::pair<const string_view, M>...>& members) {
    return _get_max_identifier_length_idx(
      members, std::make_index_sequence<sizeof...(M)>());
}
//------------------------------------------------------------------------------
export template <mapped_struct T>
constexpr auto _get_max_identifier_length(T& obj) noexcept -> span_size_t {
    return _get_max_identifier_length_map(map_data_members(obj));
}
//------------------------------------------------------------------------------
export template <typename M>
constexpr auto _get_single_max_identifier_length(
  const std::pair<const string_view, M>& m) noexcept -> span_size_t {
    return std::max(
      std::get<0>(m).size(), _get_max_identifier_length(std::get<1>(m)));
}
//------------------------------------------------------------------------------
export template <typename T>
constexpr auto max_identifier_length(T& obj) noexcept -> span_size_t {
    return _get_max_identifier_length(obj);
}
//------------------------------------------------------------------------------
// traverse mapped
//------------------------------------------------------------------------------
export template <typename T, typename Func>
    requires(not mapped_struct<T>)
void _do_traverse_mapped(basic_string_path& path, T& obj, Func& func) {
    func(static_cast<const basic_string_path&>(path), obj);
}
//------------------------------------------------------------------------------
export template <typename M, typename Func>
void _do_traverse_single_mapped(
  basic_string_path& path,
  const std::pair<const string_view, M>& m,
  Func& func);
//------------------------------------------------------------------------------
export template <typename Map, std::size_t... I, typename Func>
void _do_traverse_mapped_idx(
  basic_string_path& path,
  const Map& members,
  std::index_sequence<I...>,
  Func& func) {
    (void)(..., _do_traverse_single_mapped(path, std::get<I>(members), func));
}
//------------------------------------------------------------------------------
export template <typename... M, typename Func>
void _do_traverse_member_map(
  basic_string_path& path,
  const std::tuple<std::pair<const string_view, M>...>& members,
  Func& func) {
    _do_traverse_mapped_idx(
      path, members, std::make_index_sequence<sizeof...(M)>(), func);
}
//------------------------------------------------------------------------------
export template <typename... M, typename Func>
void _do_traverse_mapped(
  basic_string_path& path,
  const std::tuple<std::pair<const string_view, M>...>& members,
  Func& func) {
    _do_traverse_mapped_idx(
      path, members, std::make_index_sequence<sizeof...(M)>(), func);
}
//------------------------------------------------------------------------------
export template <mapped_struct T, typename Func>
void _do_traverse_mapped(basic_string_path& path, T& obj, Func& func) {
    _do_traverse_member_map(path, map_data_members(obj), func);
}
//------------------------------------------------------------------------------
export template <typename M, typename Func>
void _do_traverse_single_mapped(
  basic_string_path& path,
  const std::pair<const string_view, M>& m,
  Func& func) {
    path.push_back(std::get<0>(m));
    _do_traverse_mapped(path, std::get<1>(m), func);
    path.pop_back();
}
//------------------------------------------------------------------------------
export template <typename T, typename Func>
void traverse_mapped(basic_string_path& path, T& obj, Func func) {
    _do_traverse_mapped(path, obj, func);
}

export template <typename T, typename Func>
void traverse_mapped(T& obj, Func func) {
    basic_string_path path;
    traverse_mapped(path, obj, std::move(func));
}

//------------------------------------------------------------------------------
// visit mapped
//------------------------------------------------------------------------------
export template <typename T, typename Func>
auto visit_mapped(
  basic_string_path& path,
  const basic_string_path& pos,
  T& obj,
  Func func) -> bool {
    bool result{false};
    const auto wrapped{
      [&result, &pos, func](const basic_string_path& path, auto& v) {
          if(pos == path) {
              result = func(v);
          }
      }};
    _do_traverse_mapped(path, obj, wrapped);
    return result;
}
//------------------------------------------------------------------------------
export template <typename T, typename Func>
auto visit_mapped(const basic_string_path& pos, T& obj, Func func) -> bool {
    basic_string_path path;
    return visit_mapped(path, pos, obj, std::move(func));
}
//------------------------------------------------------------------------------
} // namespace eagine

