/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.units:si;

import eagine.core.types;
import :base;
import :unit;
import :dimension;
import :dimensions;

namespace eagine {
namespace units {
//------------------------------------------------------------------------------
// international system of units
export struct si {
    template <typename BaseDim>
    struct base_unit;
};

export template <>
struct system_id<si> : int_constant<0> {};

export template <>
struct si::base_unit<nothing_t> : nothing_t {};

export template <typename Dim>
struct si::base_unit<base::dimension<Dim>> : si::base_unit<Dim> {};
//------------------------------------------------------------------------------
namespace base {

// radian
export struct radian : unit<angle, radian> {};

// degree
export using degree =
  scaled_unit<scales::divided<scales::pi, scales::constant<180>>, radian>;

// gradian
export using gradian =
  scaled_unit<scales::divided<scales::pi, scales::constant<200>>, radian>;

// quarter
export using quarter =
  scaled_unit<scales::multiplied<scales::pi, scales::rational<1, 2>>, radian>;

// turn
export using turn =
  scaled_unit<scales::multiplied<scales::constant<2>, scales::pi>, radian>;

// kelvin
export struct kelvin : unit<temperature, kelvin> {};

// second
export struct second : unit<time, second> {};

// minute
export using minute = scaled_unit<scales::constant<60>, second>;

// hour
export using hour = scaled_unit<scales::constant<3600>, second>;

// day
export using day = scaled_unit<scales::constant<86400>, second>;

} // namespace base
//------------------------------------------------------------------------------
// units
export using second = unit<time, si>;

export using radian = unit<angle, si>;
export using degree = scaled_dim_unit<
  angle,
  bits::unit_scales<
    bits::
      uni_sca<base::radian, scales::divided<scales::pi, scales::constant<180>>>,
    nothing_t>,
  si>;

export using kelvin = unit<temperature, si>;
//------------------------------------------------------------------------------
export template <>
struct name_of<base::radian> {
    static constexpr const char mp_str[] = "radian";
};

export template <>
struct symbol_of<base::radian> {
    static constexpr const char mp_str[] = "rad";
};

export template <>
struct name_of<base::degree> {
    static constexpr const char mp_str[] = "degree";
};

export template <>
struct symbol_of<base::degree> {
    static constexpr const char mp_str[] = {char(0xC2), char(0xB0), '\0'};
};

export template <>
struct name_of<base::gradian> {
    static constexpr const char mp_str[] = "gradian";
};

export template <>
struct symbol_of<base::gradian> {
    static constexpr const char mp_str[] = "grad";
};

export template <>
struct name_of<base::quarter> {
    static constexpr const char mp_str[] = "quarter";
};

export template <>
struct symbol_of<base::quarter> {
    static constexpr const char mp_str[] = "Q";
};

export template <>
struct name_of<base::turn> {
    static constexpr const char mp_str[] = "turn";
};

export template <>
struct symbol_of<base::turn> {
    static constexpr const char mp_str[] = "r";
};

export template <>
struct si::base_unit<base::angle> : base::radian {};
//------------------------------------------------------------------------------
export template <>
struct name_of<base::kelvin> {
    static constexpr const char mp_str[] = "kelvin";
};

export template <>
struct symbol_of<base::kelvin> {
    static constexpr const char mp_str[] = "K";
};

export template <>
struct si::base_unit<base::temperature> : base::kelvin {};

// kelvin
export using kelvin = unit<temperature, si>;

// derived
export using millikelvin =
  make_scaled_base_dim_unit_t<base::scaled_unit<scales::milli, base::kelvin>, si>;

export using kilokelvin =
  make_scaled_base_dim_unit_t<base::scaled_unit<scales::kilo, base::kelvin>, si>;

// celsius
export struct celsius_conv {
    template <typename T>
    static constexpr auto to_base(const T v) noexcept -> T {
        return T(v + 273.15F);
    }

    template <typename T>
    static constexpr auto from_base(const T v) noexcept -> T {
        return T(v - 273.15F);
    }
};

export using degree_celsius = make_custom_unit_t<celsius_conv, kelvin>;

// fahrenheit
export struct fahrenheit_conv {
    template <typename T>
    static constexpr auto to_base(T v) noexcept -> T {
        return T((v - 32.F) / 1.8F + 273.15F);
    }

    template <typename T>
    static constexpr auto from_base(T v) noexcept -> T {
        return T(v * 1.8F - 459.67F);
    }
};

export using degree_fahrenheit = make_custom_unit_t<fahrenheit_conv, kelvin>;
//------------------------------------------------------------------------------
} // namespace units
} // namespace eagine
