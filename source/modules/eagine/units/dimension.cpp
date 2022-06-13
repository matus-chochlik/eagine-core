/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.units:dimension;

import eagine.core.types;
import :base;

namespace eagine::units {
//------------------------------------------------------------------------------
// multiplication
export template <typename D1, typename D2>
auto operator*(const D1, const D2) noexcept -> bits::dim_add_t<D1, D2>
    requires(is_dimension_v<D1> && is_dimension_v<D2>)
{
    return {};
}

// division
export template <typename D1, typename D2>
auto operator/(const D1, const D2) noexcept -> bits::dim_sub_t<D1, D2>
    requires(is_dimension_v<D1> && is_dimension_v<D2>)
{
    return {};
}

// base_dimension_power
template <typename Dim, typename BaseDim>
using base_dimension_power = bits::get_pow<Dim, BaseDim>;

// basic dimensions
export using angle = dimension<base::angle, 1>;
export using solid_angle = dimension<base::solid_angle, 1>;

export using length = dimension<base::length, 1>;
export using mass = dimension<base::mass, 1>;
export using time = dimension<base::time, 1>;

export using temperature = dimension<base::temperature, 1>;
export using electric_current = dimension<base::electric_current, 1>;
export using number_of_cycles = dimension<base::number_of_cycles, 1>;
export using number_of_decays = dimension<base::number_of_decays, 1>;
export using luminous_intensity = dimension<base::luminous_intensity, 1>;
export using amount_of_substance = dimension<base::amount_of_substance, 1>;
export using amount_of_information = dimension<base::amount_of_information, 1>;
//------------------------------------------------------------------------------
} // namespace eagine::units
