/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_UNITS_UNIT_SI_TEMPERATURE_HPP
#define EAGINE_UNITS_UNIT_SI_TEMPERATURE_HPP

#include "common.hpp"

namespace eagine::units {
namespace base {

// kelvin
struct kelvin : unit<temperature, kelvin> {};

} // namespace base

template <>
struct name_of<base::kelvin> {
    static constexpr const char mp_str[] = "kelvin";
};
template <>
struct symbol_of<base::kelvin> {
    static constexpr const char mp_str[] = "K";
};

// si::base_unit<temperature>
template <>
struct si::base_unit<base::temperature> : base::kelvin {};

// kelvin
using kelvin = unit<temperature, si>;

// derived
using millikelvin =
  make_scaled_base_dim_unit_t<base::scaled_unit<scales::milli, base::kelvin>, si>;

using kilokelvin =
  make_scaled_base_dim_unit_t<base::scaled_unit<scales::kilo, base::kelvin>, si>;

// celsius
struct celsius_conv {
    template <typename T>
    static constexpr auto to_base(const T v) noexcept -> T {
        return T(v + 273.15F);
    }

    template <typename T>
    static constexpr auto from_base(const T v) noexcept -> T {
        return T(v - 273.15F);
    }
};

using degree_celsius = make_custom_unit_t<celsius_conv, kelvin>;

// fahrenheit
struct fahrenheit_conv {
    template <typename T>
    static constexpr auto to_base(T v) noexcept -> T {
        return T((v - 32.F) / 1.8F + 273.15F);
    }

    template <typename T>
    static constexpr auto from_base(T v) noexcept -> T {
        return T(v * 1.8F - 459.67F);
    }
};

using degree_fahrenheit = make_custom_unit_t<fahrenheit_conv, kelvin>;

} // namespace eagine::units

#endif // EAGINE_UNITS_UNIT_SI_TEMPERATURE_HPP
