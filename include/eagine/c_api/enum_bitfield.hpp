/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_C_API_ENUM_BITFIELD_HPP
#define EAGINE_C_API_ENUM_BITFIELD_HPP

#include "enum_class.hpp"

namespace eagine::c_api {
//------------------------------------------------------------------------------
template <typename T, typename ClassList>
struct enum_bits;

template <typename T, typename... Classes>
struct enum_bits<T, mp_list<Classes...>> {
    T _bits{0};

    explicit constexpr enum_bits(const T bits) noexcept
      : _bits{bits} {}
};

template <typename T, typename TL1, typename TL2>
static constexpr auto operator|(
  const enum_value<T, TL1> a,
  const enum_value<T, TL2> b) noexcept
  requires(!mp_is_empty_v<mp_union_t<TL1, TL2>>) {
    // NOLINTNEXTLINE(hicpp-signed-bitwise)
    return enum_bits<T, mp_union_t<TL1, TL2>>{a.value | b.value};
}

template <typename T, typename TL1, typename TL2>
static constexpr auto operator|(
  const enum_bits<T, TL1> eb,
  const enum_value<T, TL2> ev) noexcept
  requires(!mp_is_empty_v<mp_union_t<TL1, TL2>>) {
    return enum_bits<T, mp_union_t<TL1, TL2>>{eb._bits | ev.value};
}
//------------------------------------------------------------------------------
template <typename EnumClass>
struct enum_bitfield {
    using value_type = typename EnumClass::value_type;

    value_type _value{0};

    constexpr enum_bitfield() noexcept = default;

    explicit constexpr enum_bitfield(const value_type value) noexcept
      : _value{value} {}

    constexpr enum_bitfield(const EnumClass e) noexcept
      : _value{e._value} {}

    template <typename Classes>
    constexpr enum_bitfield(const enum_value<value_type, Classes> ev) noexcept
      requires(mp_contains_v<Classes, EnumClass>)
      : _value{ev.value} {}

    template <typename Classes>
    constexpr enum_bitfield(const enum_bits<value_type, Classes> eb) noexcept
      requires(mp_contains_v<Classes, EnumClass>)
      : _value{eb._bits} {}

    explicit constexpr operator value_type() const noexcept {
        return _value;
    }

    auto add(const EnumClass ev) noexcept -> auto& {
        _value |= ev._value; // NOLINT(hicpp-signed-bitwise)
        return *this;
    }

    auto clear(const EnumClass ev) noexcept -> auto& {
        _value &= ~ev._value; // NOLINT(hicpp-signed-bitwise)
        return *this;
    }

    constexpr auto has(const EnumClass ev) const noexcept -> bool {
        // NOLINTNEXTLINE(hicpp-signed-bitwise)
        return (_value & ev._value) == ev._value;
    }

    friend constexpr auto operator==(
      const enum_bitfield a,
      const enum_bitfield b) noexcept {
        return a._value == b._value;
    }

    friend constexpr auto operator!=(
      const enum_bitfield a,
      const enum_bitfield b) noexcept {
        return a._value != b._value;
    }

    friend constexpr auto operator|(
      const enum_bitfield a,
      const enum_bitfield b) noexcept {
        return enum_bitfield{value_type(a._value) | value_type(b._value)};
    }

    friend constexpr auto operator&(
      const enum_bitfield a,
      const enum_bitfield b) noexcept {
        return enum_bitfield{value_type(a._value) & value_type(b._value)};
    }
};

} // namespace eagine::c_api

#endif
