/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_UNITS_UNIT_SI_ELECTRICAL_CAPACITANCE_HPP
#define EAGINE_UNITS_UNIT_SI_ELECTRICAL_CAPACITANCE_HPP

#include "../../dim/electrical_capacitance.hpp"
#include "common.hpp"
#include "electric_charge.hpp"
#include "electric_tension.hpp"

namespace eagine::units {

// farad
using farad = unit<electrical_capacitance, si>;

template <>
struct name_of<farad> {
    static constexpr const char mp_str[] = "farad";
};

template <>
struct symbol_of<farad> {
    static constexpr const char mp_str[] = "F";
};

// derived
using microfarad = make_scaled_unit_t<scales::micro, farad>;
using millifarad = make_scaled_unit_t<scales::milli, farad>;
using kilofarad = make_scaled_unit_t<scales::kilo, farad>;
// TODO

} // namespace eagine::units

#endif // EAGINE_UNITS_UNIT_SI_ELECTRICAL_CAPACITANCE_HPP
