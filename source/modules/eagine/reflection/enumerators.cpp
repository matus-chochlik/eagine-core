/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
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
export template <typename T>
struct enumerator_traits;

export template <typename T>
concept mapped_enum = requires(enumerator_traits<T> et) { et.mapping(); };
//------------------------------------------------------------------------------
export template <mapped_enum T>
constexpr auto enumerators(std::type_identity<T> tid = {}) noexcept {
    return enumerator_traits<T>{}.mapping();
}
//------------------------------------------------------------------------------
export template <mapped_enum T>
[[nodiscard]] constexpr auto enumerator_count(
  const std::type_identity<T> id = {}) noexcept -> span_size_t {
    return span_size_t(enumerators(id).size());
}
//------------------------------------------------------------------------------
export template <mapped_enum T>
[[nodiscard]] constexpr auto is_consecutive(
  const std::type_identity<T> id = {}) noexcept -> bool {
    using UT = std::underlying_type_t<T>;
    if constexpr(std::is_unsigned_v<UT>) {
        std::size_t index = 0;
        for(const auto& info : enumerators(id)) {
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
export template <mapped_enum T>
[[nodiscard]] constexpr auto is_bitset(
  const std::type_identity<T> id = {}) noexcept -> bool {
    using UT = std::underlying_type_t<T>;
    if constexpr(std::is_unsigned_v<UT>) {
        UT bit{1U};
        for(const auto& info : enumerators(id)) {
            if(bit != static_cast<UT>(info.enumerator)) {
                return false;
            }
            bit <<= 1U;
        }
        return true;
    } else {
        return false;
    }
}
//------------------------------------------------------------------------------
export template <mapped_enum T>
[[nodiscard]] constexpr auto enumerator_index(
  const T enumerator,
  const std::type_identity<T> id = {}) noexcept -> span_size_t {
    const auto enum_map{enumerators(id)};
    if constexpr(is_bitset(id)) {
        const auto pos{std::lower_bound(
          enum_map.begin(), enum_map.end(), enumerator, [](auto e, auto v) {
              return e.enumerator < v;
          })};
        return span_size(std::distance(enum_map.begin(), pos));
    } else {
        span_size_t index = 0;
        for(const auto& info : enum_map) {
            if(info.enumerator == enumerator) {
                break;
            }
            ++index;
        }
        return index;
    }
}
//------------------------------------------------------------------------------
export template <mapped_enum T>
[[nodiscard]] constexpr auto enumerator_by_index(
  const span_size_t index,
  const std::type_identity<T> id = {}) noexcept -> T {
    return enumerators(id)[span_size_t(index)].enumerator;
}
//------------------------------------------------------------------------------
export template <mapped_enum T>
[[nodiscard]] auto enumerator_name(
  const T enumerator,
  const std::type_identity<T> id = {}) noexcept -> decl_name {
    const auto enum_map{enumerators(id)};
    if constexpr(is_consecutive(id)) {
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
export template <mapped_enum T>
[[nodiscard]] auto enumerator_info(
  const T enumerator,
  const std::type_identity<T> id = {}) noexcept
  -> std::optional<enum_value_and_name<T>> {
    for(const auto& info : enumerators(id)) {
        if(info.enumerator == enumerator) {
            return {enum_value_and_name<T>{info}};
        }
    }
    return {};
}
//------------------------------------------------------------------------------
export template <mapped_enum T>
[[nodiscard]] auto from_value(
  const std::underlying_type_t<T> value,
  const std::type_identity<T> id = {}) noexcept -> std::optional<T> {
    for(const auto& info : enumerators(id)) {
        if(enumerator_value(info.enumerator, id) == value) {
            return {info.enumerator};
        }
    }
    return {};
}
//------------------------------------------------------------------------------
export template <mapped_enum T>
struct string_traits<T> {
    static auto from(const string_view name) noexcept -> std::optional<T> {
        for(const auto& info : enumerators<T>()) {
            if(are_equal(string_view(info.name), name)) {
                return {info.enumerator};
            }
        }
        return {};
    }
};
//------------------------------------------------------------------------------
export template <mapped_enum T, typename Function>
void for_each_enumerator(
  Function function,
  const std::type_identity<T> id = {}) noexcept {
    for(const auto& info : enumerators(id)) {
        function(enum_value_and_name<T>{info});
    }
}
//------------------------------------------------------------------------------
export template <mapped_enum T>
auto operator<<(std::ostream& out, T value) -> std::ostream& {
    return out << enumerator_name(value, std::type_identity<T>{});
}
//------------------------------------------------------------------------------
// bitfield
//------------------------------------------------------------------------------
export template <mapped_enum Bit>
[[nodiscard]] auto count_all_bits(const bitfield<Bit>&) noexcept
  -> span_size_t {
    return enumerator_count(std::type_identity<Bit>{});
}
//------------------------------------------------------------------------------
export template <mapped_enum Bit>
[[nodiscard]] auto count_set_bits(const bitfield<Bit> bits) noexcept
  -> span_size_t {
    span_size_t result{0};
    for(const auto& info : enumerators(std::type_identity<Bit>{})) {
        result += span_size_t(bits.has(info.enumerator));
    }
    return result;
}
//------------------------------------------------------------------------------
export template <typename Function, mapped_enum Bit>
void for_each_bit(Function function, const bitfield<Bit> bits) noexcept {
    for(const auto& info : enumerators(std::type_identity<Bit>{})) {
        function(bits, info);
    }
}
//------------------------------------------------------------------------------
} // namespace eagine
