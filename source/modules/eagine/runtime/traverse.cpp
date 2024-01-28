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
export template <typename T, typename Selector>
    requires(not mapped_struct<T, Selector>)
constexpr auto _get_max_identifier_length(T& obj, Selector) noexcept
  -> span_size_t {
    return 0;
}
//------------------------------------------------------------------------------
export template <typename M, typename Selector>
constexpr auto _get_single_max_identifier_length(
  const std::pair<const string_view, M>& m,
  Selector sel) noexcept -> span_size_t;
//------------------------------------------------------------------------------
export template <typename Map, std::size_t... I, typename Selector>
constexpr auto _get_max_identifier_length_idx(
  const Map& members,
  std::index_sequence<I...>,
  Selector sel) noexcept -> span_size_t {
    return std::max(
      _get_single_max_identifier_length(std::get<I>(members), sel)...);
}
//------------------------------------------------------------------------------
export template <typename... M, typename Selector>
constexpr auto _get_max_identifier_length_map(
  const std::tuple<std::pair<const string_view, M>...>& members,
  Selector sel) {
    return _get_max_identifier_length_idx(
      members, std::make_index_sequence<sizeof...(M)>(), sel);
}
//------------------------------------------------------------------------------
export template <typename T, typename Selector>
    requires(mapped_struct<T, Selector>)
constexpr auto _get_max_identifier_length(T& obj, Selector sel) noexcept
  -> span_size_t {
    return _get_max_identifier_length_map(map_data_members(obj), sel);
}
//------------------------------------------------------------------------------
export template <typename M, typename Selector>
constexpr auto _get_single_max_identifier_length(
  const std::pair<const string_view, M>& m,
  Selector sel) noexcept -> span_size_t {
    return std::max(
      std::get<0>(m).size(), _get_max_identifier_length(std::get<1>(m), sel));
}
//------------------------------------------------------------------------------
export template <typename T, identifier_t V>
constexpr auto max_identifier_length(T& obj, selector<V> sel) noexcept
  -> span_size_t {
    return _get_max_identifier_length(obj, sel);
}
//------------------------------------------------------------------------------
export template <typename T>
constexpr auto max_identifier_length(T& obj) noexcept -> span_size_t {
    return _get_max_identifier_length(obj, default_selector);
}
//------------------------------------------------------------------------------
// traverse mapped
//------------------------------------------------------------------------------
export template <typename T, typename Selector, typename Func>
    requires(not mapped_struct<T, Selector>)
void _do_traverse_mapped(basic_string_path& path, T& obj, Selector, Func& func) {
    func(static_cast<const basic_string_path&>(path), obj);
}
//------------------------------------------------------------------------------
export template <typename M, typename Selector, typename Func>
void _do_traverse_single_mapped(
  basic_string_path& path,
  const std::pair<const string_view, M>& m,
  Selector sel,
  Func& func);
//------------------------------------------------------------------------------
export template <typename Map, std::size_t... I, typename Selector, typename Func>
void _do_traverse_mapped_idx(
  basic_string_path& path,
  const Map& members,
  std::index_sequence<I...>,
  Selector sel,
  Func& func) {
    (void)(...,
           _do_traverse_single_mapped(path, std::get<I>(members), sel, func));
}
//------------------------------------------------------------------------------
export template <typename... M, typename Selector, typename Func>
void _do_traverse_member_map(
  basic_string_path& path,
  const std::tuple<std::pair<const string_view, M>...>& members,
  Selector sel,
  Func& func) {
    _do_traverse_mapped_idx(
      path, members, std::make_index_sequence<sizeof...(M)>(), sel, func);
}
//------------------------------------------------------------------------------
export template <typename... M, typename Selector, typename Func>
void _do_traverse_mapped(
  basic_string_path& path,
  const std::tuple<std::pair<const string_view, M>...>& members,
  Selector sel,
  Func& func) {
    _do_traverse_mapped_idx(
      path, members, std::make_index_sequence<sizeof...(M)>(), sel, func);
}
//------------------------------------------------------------------------------
export template <typename T, typename Selector, typename Func>
    requires(mapped_struct<T, Selector>)
void _do_traverse_mapped(
  basic_string_path& path,
  T& obj,
  Selector sel,
  Func& func) {
    _do_traverse_member_map(path, map_data_members(obj), sel, func);
}
//------------------------------------------------------------------------------
export template <typename M, typename Selector, typename Func>
void _do_traverse_single_mapped(
  basic_string_path& path,
  const std::pair<const string_view, M>& m,
  Selector sel,
  Func& func) {
    path.push_back(std::get<0>(m));
    _do_traverse_mapped(path, std::get<1>(m), sel, func);
    path.pop_back();
}
//------------------------------------------------------------------------------
export template <typename T, identifier_t V, typename Func>
void traverse_mapped(
  basic_string_path& path,
  T& obj,
  selector<V> sel,
  Func func) {
    _do_traverse_mapped(path, obj, sel, func);
}

export template <typename T, identifier_t V, typename Func>
void traverse_mapped(T& obj, selector<V> sel, Func func) {
    basic_string_path path;
    traverse_mapped(path, obj, sel, std::move(func));
}

export template <typename T, typename Func>
void traverse_mapped(T& obj, Func func) {
    traverse_mapped(obj, default_selector, std::move(func));
}
//------------------------------------------------------------------------------
// visit mapped
//------------------------------------------------------------------------------
export template <typename T, identifier_t V, typename Func>
auto visit_mapped(
  basic_string_path& path,
  const basic_string_path& pos,
  T& obj,
  selector<V> sel,
  Func func) -> bool {
    bool result{false};
    const auto wrapped{
      [&result, &pos, func](const basic_string_path& path, auto& v) {
          if(pos == path) {
              result = func(v);
          }
      }};
    _do_traverse_mapped(path, obj, sel, wrapped);
    return result;
}
//------------------------------------------------------------------------------
export template <typename T, identifier_t V, typename Func>
auto visit_mapped(
  const basic_string_path& pos,
  T& obj,
  selector<V> sel,
  Func func) -> bool {
    basic_string_path path;
    return visit_mapped(path, pos, obj, sel, std::move(func));
}
//------------------------------------------------------------------------------
export template <typename T, typename Func>
auto visit_mapped(const basic_string_path& pos, T& obj, Func func) -> bool {
    return visit_mapped(pos, obj, default_selector, std::move(func));
}
//------------------------------------------------------------------------------
} // namespace eagine

