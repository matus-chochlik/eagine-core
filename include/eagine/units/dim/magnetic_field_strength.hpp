/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_UNITS_DIM_MAGNETIC_FIELD_STRENGTH_HPP
#define EAGINE_UNITS_DIM_MAGNETIC_FIELD_STRENGTH_HPP

#include "area.hpp"
#include "magnetic_flux.hpp"

namespace eagine::units {

using magnetic_field_strength = decltype(magnetic_flux() / area());

template <>
struct name_of<magnetic_field_strength> {
    static constexpr const char mp_str[] = "magnetic field strength";
};

} // namespace eagine::units

#endif // EAGINE_UNITS_DIM_MAGNETIC_FIELD_STRENGTH_HPP
