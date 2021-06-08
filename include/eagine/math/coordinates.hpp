/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#ifndef EAGINE_MATH_COORDINATES_HPP
#define EAGINE_MATH_COORDINATES_HPP

#include "../quantities.hpp"
#include "../valid_if/nonnegative.hpp"
#include "../valid_if/positive.hpp"
#include "functions.hpp"
#include "vector.hpp"
#include <tuple>

namespace eagine::math {
//------------------------------------------------------------------------------
/// @brief Class representing unit spherical coordinate.
/// @ingroup math
template <typename T, bool V>
class unit_spherical_coordinate {
public:
    /// @brief Default constructor.
    constexpr unit_spherical_coordinate() noexcept = default;

    /// @brief Constructor initializing the azimuth and elevation.
    constexpr unit_spherical_coordinate(
      radians_t<T> azimuth,
      radians_t<T> elevation) noexcept
      : _azim{azimuth}
      , _elev{elevation} {}

    /// @brief Returns the azimuth.
    auto azimuth() const noexcept -> radians_t<T> {
        return _azim;
    }

    /// @brief Returns the elevation.
    auto elevation() const noexcept -> radians_t<T> {
        return _elev;
    }

private:
    radians_t<T> _azim{T(0)};
    radians_t<T> _elev{T(0)};
};
//------------------------------------------------------------------------------
/// @brief Converts unit spherical coordinate to cartesian vector.
/// @ingroup math
template <typename T, bool V>
auto to_cartesian(unit_spherical_coordinate<T, V> c) noexcept
  -> vector<T, 3, V> {
    using std::abs;
    return vector<T, 3, V>::make(
      cos(c.azimuth()) * abs(cos(c.elevation())),
      sin(c.elevation()),
      -sin(c.azimuth()) * abs(cos(c.elevation())));
}
//------------------------------------------------------------------------------
/// @brief Converts cartesian vector to unit spherical coordinate.
/// @ingroup math
template <typename T, bool V>
auto to_unit_spherical(vector<T, 3, V> v) noexcept
  -> unit_spherical_coordinate<T, V> {
    using R = unit_spherical_coordinate<T, V>;
    using std::abs;
    return abs(v.y()) > T(0) ? R(-arctan(v.z(), v.x()), arcsin(v.y()))
                             : R(radians_t<T>(0), arcsin(v.y()));
}
//------------------------------------------------------------------------------
} // namespace eagine::math

#endif // EAGINE_MATH_COORDINATES_HPP
