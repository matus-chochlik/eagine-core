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

export struct steradian : unit<solid_angle, steradian> {};

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

export struct mole : unit<amount_of_substance, mole> {};

export struct count_of_cycles : unit<number_of_cycles, count_of_cycles> {};

export struct count_of_decays : unit<number_of_decays, count_of_decays> {};

export struct meter : unit<length, meter> {};

export struct gram : unit<mass, gram> {};

export using tonne = scaled_unit<scales::mega, gram>;

export struct candela : unit<luminous_intensity, candela> {};

export struct ampere : unit<electric_current, ampere> {};

} // namespace base
//------------------------------------------------------------------------------
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
struct name_of<base::second> {
    static constexpr const char mp_str[] = "second";
};

export template <>
struct symbol_of<base::second> {
    static constexpr const char mp_str[] = "s";
};

export template <>
struct name_of<base::minute> {
    static constexpr const char mp_str[] = "minute";
};

export template <>
struct symbol_of<base::minute> {
    static constexpr const char mp_str[] = "min";
};

export template <>
struct name_of<base::hour> {
    static constexpr const char mp_str[] = "hour";
};

export template <>
struct symbol_of<base::hour> {
    static constexpr const char mp_str[] = "h";
};

export template <>
struct si::base_unit<base::time> : base::second {};
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
struct name_of<base::steradian> {
    static constexpr const char mp_str[] = "steradian";
};

export template <>
struct symbol_of<base::steradian> {
    static constexpr const char mp_str[] = "sr";
};

export template <>
struct si::base_unit<base::solid_angle> : base::steradian {};

export using steradian = unit<solid_angle, si>;
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
export template <>
struct name_of<base::mole> {
    static constexpr const char mp_str[] = "mole";
};
export template <>
struct symbol_of<base::mole> {
    static constexpr const char mp_str[] = "mol";
};

export template <>
struct si::base_unit<base::amount_of_substance> : base::mole {};

export using mole = unit<amount_of_substance, si>;

export using kilomole =
  make_scaled_base_dim_unit_t<base::scaled_unit<scales::kilo, base::mole>, si>;
//------------------------------------------------------------------------------
export template <>
struct si::base_unit<base::number_of_cycles> : base::count_of_cycles {};

export using hertz = unit<frequency, si>;

export template <>
struct name_of<hertz> {
    static constexpr const char mp_str[] = "hertz";
};

export template <>
struct symbol_of<hertz> {
    static constexpr const char mp_str[] = "Hz";
};

export using kilohertz = make_scaled_unit_t<scales::kilo, hertz>;
export using megahertz = make_scaled_unit_t<scales::mega, hertz>;
export using gigahertz = make_scaled_unit_t<scales::giga, hertz>;
//------------------------------------------------------------------------------
export template <>
struct si::base_unit<base::number_of_decays> : base::count_of_decays {};

export using count_of_decays = unit<number_of_decays, si>;

export using becquerel = unit<radioactivity, si>;

export template <>
struct name_of<becquerel> {
    static constexpr const char mp_str[] = "becquerel";
};

export template <>
struct symbol_of<becquerel> {
    static constexpr const char mp_str[] = "Bq";
};

export using kilobecquerel = make_scaled_unit_t<scales::kilo, becquerel>;
export using megabecquerel = make_scaled_unit_t<scales::mega, becquerel>;
export using gigabecquerel = make_scaled_unit_t<scales::giga, becquerel>;
//------------------------------------------------------------------------------
export template <>
struct name_of<base::meter> {
    static constexpr const char mp_str[] = "meter";
};
export template <>
struct symbol_of<base::meter> {
    static constexpr const char mp_str[] = "m";
};

export template <>
struct si::base_unit<base::length> : base::meter {};

export using meter = unit<length, si>;

export using kilometer =
  make_scaled_base_dim_unit_t<base::scaled_unit<scales::kilo, base::meter>, si>;

export using decimeter =
  make_scaled_base_dim_unit_t<base::scaled_unit<scales::deci, base::meter>, si>;

export using centimeter =
  make_scaled_base_dim_unit_t<base::scaled_unit<scales::centi, base::meter>, si>;

export using millimeter =
  make_scaled_base_dim_unit_t<base::scaled_unit<scales::milli, base::meter>, si>;
//------------------------------------------------------------------------------
export template <>
struct name_of<base::gram> {
    static constexpr const char mp_str[] = "gram";
};
export template <>
struct symbol_of<base::gram> {
    static constexpr const char mp_str[] = "g";
};

export template <>
struct name_of<base::tonne> {
    static constexpr const char mp_str[] = "tonne";
};

export template <>
struct symbol_of<base::tonne> {
    static constexpr const char mp_str[] = "t";
};

export template <>
struct si::base_unit<base::mass>
  : base::scaled_unit<scales::kilo, base::gram> {};

export using kilogram = unit<mass, si>;

export using gram =
  make_scaled_base_dim_unit_t<base::scaled_unit<scales::one, base::gram>, si>;

export using milligram =
  make_scaled_base_dim_unit_t<base::scaled_unit<scales::milli, base::gram>, si>;

export using tonne = make_scaled_base_dim_unit_t<base::tonne, si>;
//------------------------------------------------------------------------------
export template <>
struct name_of<base::candela> {
    static constexpr const char mp_str[] = "candela";
};

export template <>
struct symbol_of<base::candela> {
    static constexpr const char mp_str[] = "cd";
};

export template <>
struct si::base_unit<base::luminous_intensity> : base::candela {};

export using candela = unit<luminous_intensity, si>;

export using kilocandela =
  make_scaled_base_dim_unit_t<base::scaled_unit<scales::kilo, base::candela>, si>;
//------------------------------------------------------------------------------
export using newton = unit<force, si>;

export template <>
struct name_of<newton> {
    static constexpr const char mp_str[] = "newton";
};

export template <>
struct symbol_of<newton> {
    static constexpr const char mp_str[] = "N";
};

export using kilonewton = make_scaled_unit_t<scales::kilo, newton>;
export using meganewton = make_scaled_unit_t<scales::mega, newton>;
//------------------------------------------------------------------------------
export using joule = unit<energy, si>;

export template <>
struct name_of<joule> {
    static constexpr const char mp_str[] = "joule";
};

export template <>
struct symbol_of<joule> {
    static constexpr const char mp_str[] = "J";
};

export using kilojoule = make_scaled_unit_t<scales::kilo, joule>;
export using megajoule = make_scaled_unit_t<scales::mega, joule>;
//------------------------------------------------------------------------------
export using watt = unit<power, si>;

export template <>
struct name_of<watt> {
    static constexpr const char mp_str[] = "watt";
};

export template <>
struct symbol_of<watt> {
    static constexpr const char mp_str[] = "W";
};

export using kilowatt = make_scaled_unit_t<scales::kilo, watt>;
export using megawatt = make_scaled_unit_t<scales::mega, watt>;
//------------------------------------------------------------------------------
export using pascal = unit<pressure, si>;

export template <>
struct name_of<pascal> {
    static constexpr const char mp_str[] = "pascal";
};

export template <>
struct symbol_of<pascal> {
    static constexpr const char mp_str[] = "Pa";
};

export using kilopascal = make_scaled_unit_t<scales::kilo, pascal>;
export using megapascal = make_scaled_unit_t<scales::mega, pascal>;
//------------------------------------------------------------------------------
export template <>
struct name_of<base::ampere> {
    static constexpr const char mp_str[] = "ampere";
};
export template <>
struct symbol_of<base::ampere> {
    static constexpr const char mp_str[] = "A";
};

export template <>
struct si::base_unit<base::electric_current> : base::ampere {};

export using ampere = unit<electric_current, si>;

export using kiloampere =
  make_scaled_base_dim_unit_t<base::scaled_unit<scales::kilo, base::ampere>, si>;

export using milliampere =
  make_scaled_base_dim_unit_t<base::scaled_unit<scales::milli, base::ampere>, si>;
//------------------------------------------------------------------------------
export using coulomb = unit<electric_charge, si>;

export template <>
struct name_of<coulomb> {
    static constexpr const char mp_str[] = "coulomb";
};

export template <>
struct symbol_of<coulomb> {
    static constexpr const char mp_str[] = "C";
};

export using millicoulomb = make_scaled_unit_t<scales::milli, coulomb>;
export using kilocoulomb = make_scaled_unit_t<scales::kilo, coulomb>;
//------------------------------------------------------------------------------
export using volt = unit<electric_tension, si>;

export template <>
struct name_of<volt> {
    static constexpr const char mp_str[] = "volt";
};

export template <>
struct symbol_of<volt> {
    static constexpr const char mp_str[] = "V";
};

export using millivolt = make_scaled_unit_t<scales::milli, volt>;
export using kilovolt = make_scaled_unit_t<scales::kilo, volt>;
//------------------------------------------------------------------------------
export using farad = unit<electrical_capacitance, si>;

export template <>
struct name_of<farad> {
    static constexpr const char mp_str[] = "farad";
};

export template <>
struct symbol_of<farad> {
    static constexpr const char mp_str[] = "F";
};

export using microfarad = make_scaled_unit_t<scales::micro, farad>;
export using millifarad = make_scaled_unit_t<scales::milli, farad>;
export using kilofarad = make_scaled_unit_t<scales::kilo, farad>;
//------------------------------------------------------------------------------
export using siemens = unit<electrical_conductance, si>;

export template <>
struct name_of<siemens> {
    static constexpr const char mp_str[] = "siemens";
};

export template <>
struct symbol_of<siemens> {
    static constexpr const char mp_str[] = "S";
};

export using millisiemens = make_scaled_unit_t<scales::milli, siemens>;
//------------------------------------------------------------------------------
export using ohm = unit<electrical_resistance, si>;

export template <>
struct name_of<ohm> {
    static constexpr const char mp_str[] = "ohm";
};

export template <>
struct symbol_of<ohm> {
    static constexpr const char mp_str[] = {char(0xCE), char(0xA9), '\0'};
};

export using milliohm = make_scaled_unit_t<scales::milli, ohm>;
export using kiloohm = make_scaled_unit_t<scales::kilo, ohm>;
//------------------------------------------------------------------------------
export using weber = unit<magnetic_flux, si>;

export template <>
struct name_of<weber> {
    static constexpr const char mp_str[] = "weber";
};

export template <>
struct symbol_of<weber> {
    static constexpr const char mp_str[] = "Wb";
};

export using milliweber = make_scaled_unit_t<scales::milli, weber>;
export using kiloweber = make_scaled_unit_t<scales::kilo, weber>;
//------------------------------------------------------------------------------
export using tesla = unit<magnetic_field_strength, si>;

export template <>
struct name_of<tesla> {
    static constexpr const char mp_str[] = "tesla";
};

export template <>
struct symbol_of<tesla> {
    static constexpr const char mp_str[] = "T";
};

export using millitesla = make_scaled_unit_t<scales::milli, tesla>;
//------------------------------------------------------------------------------
} // namespace units
} // namespace eagine
