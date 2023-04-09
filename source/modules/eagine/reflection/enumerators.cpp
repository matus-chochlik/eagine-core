/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.reflection:enumerators;

import std;
import eagine.core.types;
import eagine.core.memory;
import :decl_name;

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
auto enumerators(Selector s) noexcept {
    return enumerator_mapping(std::type_identity<T>{}, s);
}

export template <typename T>
auto enumerators() noexcept {
    return enumerators<T>(default_selector);
}
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
export template <typename T, identifier_t V>
    requires(mapped_enum<T, selector<V>>)
[[nodiscard]] constexpr auto enumerator_count(
  const std::type_identity<T> id,
  const selector<V> sel) noexcept -> span_size_t {
    return span_size_t(enumerator_mapping(id, sel).size());
}
//------------------------------------------------------------------------------
export template <typename T>
[[nodiscard]] constexpr auto enumerator_count(
  const std::type_identity<T> id) noexcept {
    return enumerator_count(id, default_selector);
}
//------------------------------------------------------------------------------
export template <typename T, identifier_t V>
    requires(mapped_enum<T, selector<V>>)
[[nodiscard]] constexpr auto is_consecutive(
  const std::type_identity<T> id,
  const selector<V> sel) noexcept -> bool {
    using UT = std::underlying_type_t<T>;
    if constexpr(std::is_unsigned_v<UT>) {
        std::size_t index = 0;
        for(const auto& info : enumerator_mapping(id, sel)) {
            if(index != std::size_t(static_cast<UT>(info.enumerator))) {
                return false;
            }
            ++index;
        }
        return true;
    } else {
        return false;
    }
}
//------------------------------------------------------------------------------
export template <typename T>
[[nodiscard]] constexpr auto is_consecutive(
  const std::type_identity<T> id) noexcept -> bool {
    return is_consecutive(id, default_selector);
}
//------------------------------------------------------------------------------
export template <typename T, identifier_t V>
    requires(mapped_enum<T, selector<V>>)
[[nodiscard]] auto enumerator_name(
  const T enumerator,
  const std::type_identity<T> id,
  const selector<V> sel) noexcept -> decl_name {
    const auto enum_map{enumerator_mapping(id, sel)};
    if constexpr(is_consecutive(id, sel)) {
        using UT = std::underlying_type_t<T>;
        const auto index{std::size_t(static_cast<UT>(enumerator))};
        if(index < enum_map.size()) [[likely]] {
            return enum_map[index].name;
        }
    } else {
        for(const auto& info : enum_map) {
            if(info.enumerator == enumerator) {
                return info.name;
            }
        }
    }
    return {};
}
//------------------------------------------------------------------------------
export template <typename T>
[[nodiscard]] constexpr auto enumerator_name(
  const T value,
  const std::type_identity<T> id = {}) noexcept {
    return enumerator_name(value, id, default_selector);
}
//------------------------------------------------------------------------------
export template <typename T>
[[nodiscard]] constexpr auto enumerator_value(
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
export template <typename T, identifier_t V>
    requires(mapped_enum<T, selector<V>>)
[[nodiscard]] auto enumerator_info(
  const T enumerator,
  const std::type_identity<T> id,
  const selector<V> sel) noexcept -> std::optional<enum_value_and_name<T>> {
    for(const auto& info : enumerator_mapping(id, sel)) {
        if(info.enumerator == enumerator) {
            return {enum_value_and_name<T>{info}};
        }
    }
    return {};
}
//------------------------------------------------------------------------------
export template <typename T>
[[nodiscard]] auto enumerator_info(
  const T enumerator,
  const std::type_identity<T> id = {}) noexcept {
    return enumerator_info(enumerator, id, default_selector);
}
//------------------------------------------------------------------------------
export template <typename T, identifier_t V>
    requires(mapped_enum<T, selector<V>>)
[[nodiscard]] auto from_value(
  const std::underlying_type_t<T> value,
  const std::type_identity<T> id,
  const selector<V> sel) noexcept -> std::optional<T> {
    for(const auto& info : enumerator_mapping(id, sel)) {
        if(enumerator_value(info.enumerator, id) == value) {
            return {info.enumerator};
        }
    }
    return {};
}
//------------------------------------------------------------------------------
export template <typename T>
[[nodiscard]] constexpr auto from_value(
  const std::underlying_type_t<T> value,
  const std::type_identity<T> id = {}) noexcept {
    return from_value(value, id, default_selector);
}
//------------------------------------------------------------------------------
export template <typename T, identifier_t V>
    requires(mapped_enum<T, selector<V>>)
[[nodiscard]] auto from_string(
  const string_view name,
  const std::type_identity<T> id,
  const selector<V> sel) noexcept -> std::optional<T> {
    for(const auto& info : enumerator_mapping(id, sel)) {
        if(are_equal(string_view(info.name), name)) {
            return {info.enumerator};
        }
    }
    return {};
}
//------------------------------------------------------------------------------
export template <typename Function, typename T, identifier_t V>
    requires(mapped_enum<T, selector<V>>)
void for_each_enumerator(
  Function function,
  const std::type_identity<T> id,
  const selector<V> sel) noexcept {
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
// bitfield
//------------------------------------------------------------------------------
export template <typename Bit, typename Selector>
[[nodiscard]] auto count_all_bits(
  const bitfield<Bit>&,
  const Selector sel) noexcept -> span_size_t
    requires(mapped_enum<Bit, Selector>)
{
    return enumerator_count(std::type_identity<Bit>{}, sel);
}
//------------------------------------------------------------------------------
export template <typename Bit>
[[nodiscard]] auto count_all_bits(const bitfield<Bit>&) noexcept -> span_size_t
    requires(mapped_enum<Bit, default_selector_t>)
{
    return enumerator_count(std::type_identity<Bit>{}, default_selector);
}
//------------------------------------------------------------------------------
export template <typename Bit, typename Selector>
[[nodiscard]] auto count_set_bits(
  const bitfield<Bit> bits,
  const Selector sel) noexcept -> span_size_t
    requires(mapped_enum<Bit, Selector>)
{
    span_size_t result{0};
    for(const auto& info : enumerator_mapping(std::type_identity<Bit>{}, sel)) {
        result += (bits.has(info.enumerator) ? 1 : 0);
    }
    return result;
}
//------------------------------------------------------------------------------
export template <default_mapped_enum Bit>
[[nodiscard]] auto count_set_bits(const bitfield<Bit> bits) noexcept
  -> span_size_t {
    return count_set_bits(bits, default_selector);
}
//------------------------------------------------------------------------------
export template <typename Function, typename Bit, typename Selector>
void for_each_bit(
  Function function,
  const bitfield<Bit> bits,
  const Selector sel) noexcept
    requires(mapped_enum<Bit, Selector>)
{
    for(const auto& info : enumerator_mapping(std::type_identity<Bit>{}, sel)) {
        function(bits, info);
    }
}
//------------------------------------------------------------------------------
export template <typename Function, default_mapped_enum Bit>
void for_each_bit(Function function, const bitfield<Bit> bits) noexcept {
    for_each_bit(function, bits, default_selector);
}
//------------------------------------------------------------------------------
} // namespace eagine
