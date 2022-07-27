/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.reflection:enumerators;

import eagine.core.types;
import eagine.core.memory;
import :decl_name;
import <array>;
import <cstdint>;
import <optional>;
import <type_traits>;

namespace eagine {
//------------------------------------------------------------------------------
export template <typename T>
struct name_and_enumerator {
    constexpr name_and_enumerator(const decl_name n, const T e) noexcept
      : name{n}
      , enumerator{e} {}

    const decl_name name;
    const T enumerator;
};

export template <typename T>
name_and_enumerator(const decl_name, const T) -> name_and_enumerator<T>;
//------------------------------------------------------------------------------
export template <typename T, std::size_t N>
using enumerator_map_type = std::array<const name_and_enumerator<T>, N>;
//------------------------------------------------------------------------------
export template <typename T, typename Selector>
concept mapped_enum = requires(std::type_identity<T> tid, Selector s) {
                          enumerator_mapping(tid, s);
                      };

export template <typename T>
concept default_mapped_enum = requires(std::type_identity<T> tid) {
                                  enumerator_mapping(tid, default_selector);
                              };
//------------------------------------------------------------------------------
export template <typename T, typename Selector>
    requires(mapped_enum<T, Selector>)
constexpr auto enumerator_count(
  const std::type_identity<T> id,
  const Selector sel) noexcept -> span_size_t {
    return span_size_t(enumerator_mapping(id, sel).size());
}
//------------------------------------------------------------------------------
export template <typename T>
constexpr auto enumerator_count(const std::type_identity<T> id) noexcept {
    return enumerator_count(id, default_selector);
}
//------------------------------------------------------------------------------
export template <typename T>
constexpr auto enumerator_name(
  const T value,
  const std::type_identity<T> id = {}) noexcept {
    return enumerator_name(value, id, default_selector);
}
//------------------------------------------------------------------------------
export template <typename T, typename Selector>
    requires(mapped_enum<T, Selector>)
auto enumerator_name(
  const T enumerator,
  const std::type_identity<T> id,
  const Selector sel) noexcept -> decl_name {
    for(const auto& info : enumerator_mapping(id, sel)) {
        if(info.enumerator == enumerator) {
            return info.name;
        }
    }
    return {};
}
//------------------------------------------------------------------------------
export template <typename T>
constexpr auto enumerator_value(
  const T value,
  const std::type_identity<T> = {}) noexcept {
    return static_cast<std::underlying_type_t<T>>(value);
}
//------------------------------------------------------------------------------
export template <bool Signed>
struct enum_int_value_and_name {
    using value_type =
      std::conditional_t<Signed, std::intmax_t, std::uintmax_t>;

    constexpr enum_int_value_and_name() noexcept = default;

    template <typename T>
    constexpr enum_int_value_and_name(const name_and_enumerator<T>& src) noexcept
      : name{src.name}
      , value{static_cast<value_type>(src.enumerator)} {}

    const string_view name{};
    const value_type value{0};
};
//------------------------------------------------------------------------------
export using signed_enum_value_and_name = enum_int_value_and_name<true>;
export using unsigned_enum_value_and_name = enum_int_value_and_name<false>;
//------------------------------------------------------------------------------
export template <typename T>
using enum_value_and_name =
  enum_int_value_and_name<std::is_signed_v<std::underlying_type_t<T>>>;
//------------------------------------------------------------------------------
export template <typename T, typename Selector>
    requires(mapped_enum<T, Selector>)
auto enumerator_info(
  const T enumerator,
  const std::type_identity<T> id,
  const Selector sel) noexcept -> std::optional<enum_value_and_name<T>> {
    for(const auto& info : enumerator_mapping(id, sel)) {
        if(info.enumerator == enumerator) {
            return {enum_value_and_name<T>{info}};
        }
    }
    return {};
}
//------------------------------------------------------------------------------
export template <typename T>
auto enumerator_info(
  const T enumerator,
  const std::type_identity<T> id = {}) noexcept {
    return enumerator_info(enumerator, id, default_selector);
}
//------------------------------------------------------------------------------
export template <typename T, typename Selector>
    requires(mapped_enum<T, Selector>)
auto from_value(
  const std::underlying_type_t<T> value,
  const std::type_identity<T> id,
  const Selector sel) noexcept -> std::optional<T> {
    for(const auto& info : enumerator_mapping(id, sel)) {
        if(enumerator_value(info.enumerator, id) == value) {
            return {info.enumerator};
        }
    }
    return {};
}
//------------------------------------------------------------------------------
export template <typename T>
constexpr auto from_value(
  const std::underlying_type_t<T> value,
  const std::type_identity<T> id = {}) noexcept {
    return from_value(value, id, default_selector);
}
//------------------------------------------------------------------------------
export template <typename T, typename Selector>
    requires(mapped_enum<T, Selector>)
auto from_string(
  const string_view name,
  const std::type_identity<T> id,
  const Selector sel) noexcept -> std::optional<T> {
    for(const auto& info : enumerator_mapping(id, sel)) {
        if(are_equal(string_view(info.name), name)) {
            return {info.enumerator};
        }
    }
    return {};
}
//------------------------------------------------------------------------------
export template <typename Function, typename T, typename Selector>
    requires(mapped_enum<T, Selector>)
void for_each_enumerator(
  Function function,
  const std::type_identity<T> id,
  const Selector sel) noexcept {
    for(const auto& info : enumerator_mapping(id, sel)) {
        function(enum_value_and_name<T>{info});
    }
}
//------------------------------------------------------------------------------
export template <typename Function, default_mapped_enum T>
void for_each_enumerator(
  Function function,
  const std::type_identity<T> id) noexcept {
    for_each_enumerator(function, id, default_selector);
}
//------------------------------------------------------------------------------
} // namespace eagine
