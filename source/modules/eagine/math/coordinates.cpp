/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.math:coordinates;

import std;
import eagine.core.valid_if;
import eagine.core.units;
import :functions;
import :vector;

namespace eagine::math {
//------------------------------------------------------------------------------
/// @brief Class representing unit spherical coordinate.
/// @ingroup math
export template <typename T, bool V = true>
class unit_spherical_coordinate {
public:
    /// @brief Default constructor.
    constexpr unit_spherical_coordinate() noexcept = default;

    /// @brief Constructor initializing the azimuth and elevation.
    constexpr unit_spherical_coordinate(
      const radians_t<T> azimuth,
      const radians_t<T> elevation) noexcept
      : _azim{azimuth}
      , _elev{elevation} {}

    /// @brief Returns the azimuth.
    [[nodiscard]] constexpr auto azimuth() const noexcept -> radians_t<T> {
        return _azim;
    }

    /// @brief Returns the elevation.
    [[nodiscard]] constexpr auto elevation() const noexcept -> radians_t<T> {
        return _elev;
    }

private:
    radians_t<T> _azim{T(0)};
    radians_t<T> _elev{T(0)};
};
//------------------------------------------------------------------------------
/// @brief Converts unit spherical coordinate to cartesian vector.
/// @ingroup math
export template <typename T, bool V>
[[nodiscard]] constexpr auto to_cartesian(
  const unit_spherical_coordinate<T, V> c) noexcept -> vector<T, 3, V> {
    using std::abs;
    return vector<T, 3, V>(
      cos(c.azimuth()) * abs(cos(c.elevation())),
      sin(c.elevation()),
      -sin(c.azimuth()) * abs(cos(c.elevation())));
}
//------------------------------------------------------------------------------
/// @brief Converts cartesian vector to unit spherical coordinate.
/// @ingroup math
export template <typename T, bool V>
[[nodiscard]] constexpr auto to_unit_spherical(const vector<T, 3, V> v) noexcept
  -> unit_spherical_coordinate<T, V> {
    using R = unit_spherical_coordinate<T, V>;
    using std::abs;
    return abs(v.y()) > T(0) ? R(-arctan(v.z(), v.x()), arcsin(v.y()))
                             : R(radians_t<T>(0), arcsin(v.y()));
}
//------------------------------------------------------------------------------
} // namespace eagine::math

