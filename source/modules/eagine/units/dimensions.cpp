/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.units:dimensions;

import eagine.core.types;
import :base;
import :dimension;

namespace eagine::units {
//------------------------------------------------------------------------------
export using frequency = decltype(number_of_cycles() / time());

export template <>
struct name_of<frequency> {
    static constexpr const char mp_str[] = "frequency";
};
//------------------------------------------------------------------------------
export using radioactivity = decltype(number_of_decays() / time());

export template <>
struct name_of<radioactivity> {
    static constexpr const char mp_str[] = "radioactivity";
};
//------------------------------------------------------------------------------
export using area = decltype(length() * length());

export template <>
struct name_of<area> {
    static constexpr const char mp_str[] = "area";
};
//------------------------------------------------------------------------------
export using volume = decltype(length() * length() * length());

export template <>
struct name_of<volume> {
    static constexpr const char mp_str[] = "volume";
};
//------------------------------------------------------------------------------
export using mass_density = decltype(mass() / volume());

export template <>
struct name_of<mass_density> {
    static constexpr const char mp_str[] = "mass density";
};
//------------------------------------------------------------------------------
export using angular_velocity = decltype(angle() / time());

export template <>
struct name_of<angular_velocity> {
    static constexpr const char mp_str[] = "angular velocity";
};
//------------------------------------------------------------------------------
export using velocity = decltype(length() / time());

export template <>
struct name_of<velocity> {
    static constexpr const char mp_str[] = "velocity";
};
//------------------------------------------------------------------------------
export using acceleration = decltype(velocity() / time());

export template <>
struct name_of<acceleration> {
    static constexpr const char mp_str[] = "acceleration";
};
//------------------------------------------------------------------------------
export using momentum = decltype(velocity() * mass());

export template <>
struct name_of<momentum> {
    static constexpr const char mp_str[] = "momentum";
};
//------------------------------------------------------------------------------
export using force = decltype(acceleration() * mass());

export template <>
struct name_of<force> {
    static constexpr const char mp_str[] = "force";
};
//------------------------------------------------------------------------------
export using energy = decltype(force() * length());

export template <>
struct name_of<energy> {
    static constexpr const char mp_str[] = "energy";
};
//------------------------------------------------------------------------------
export using power = decltype(energy() / time());

export template <>
struct name_of<power> {
    static constexpr const char mp_str[] = "power";
};
//------------------------------------------------------------------------------
export using pressure = decltype(force() / area());

export template <>
struct name_of<pressure> {
    static constexpr const char mp_str[] = "pressure";
};
//------------------------------------------------------------------------------
export using electric_charge = decltype(electric_current() * time());

export template <>
struct name_of<electric_charge> {
    static constexpr const char mp_str[] = "electric charge";
};
//------------------------------------------------------------------------------
export using electric_tension = decltype(power() / electric_current());

export template <>
struct name_of<electric_tension> {
    static constexpr const char mp_str[] = "electric tension";
};
//------------------------------------------------------------------------------
export using electrical_capacitance =
  decltype(electric_charge() / electric_tension());

export template <>
struct name_of<electrical_capacitance> {
    static constexpr const char mp_str[] = "electrical capacitance";
};
//------------------------------------------------------------------------------
export using electrical_conductance =
  decltype(electric_current() / electric_tension());

export template <>
struct name_of<electrical_conductance> {
    static constexpr const char mp_str[] = "electrical conductance";
};
//------------------------------------------------------------------------------
export using electrical_resistance =
  decltype(electric_tension() / electric_current());

export template <>
struct name_of<electrical_resistance> {
    static constexpr const char mp_str[] = "electrical resistance";
};
//------------------------------------------------------------------------------
export using magnetic_flux = decltype(energy() / electric_current());

export template <>
struct name_of<magnetic_flux> {
    static constexpr const char mp_str[] = "magnetic flux";
};
//------------------------------------------------------------------------------
export using magnetic_field_strength = decltype(magnetic_flux() / area());

export template <>
struct name_of<magnetic_field_strength> {
    static constexpr const char mp_str[] = "magnetic field strength";
};
//------------------------------------------------------------------------------
export using inductance = decltype(magnetic_flux() / electric_current());

export template <>
struct name_of<inductance> {
    static constexpr const char mp_str[] = "inductance";
};
//------------------------------------------------------------------------------
} // namespace eagine::units
