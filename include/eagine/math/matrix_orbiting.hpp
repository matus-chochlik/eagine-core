/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#ifndef EAGINE_MATH_MATRIX_ORBITING_HPP
#define EAGINE_MATH_MATRIX_ORBITING_HPP

#include "../quantities.hpp"
#include "matrix_ctr.hpp"

namespace eagine::math {

// orbiting_y_up
template <typename X>
class orbiting_y_up;

// is_matrix_constructor<orbiting_y_up>
template <typename T, int N, bool RM, bool V>
struct is_matrix_constructor<orbiting_y_up<matrix<T, N, N, RM, V>>>
  : std::true_type {};

/// @brief Implements constructor of orbiting matrix used for camera transformations.
/// @ingroup math
/// @see matrix_orbiting_y_up
///
/// This implementation assumes the positive y axis points up.
///
/// This implementation assumes the positive y axis points up.
/// @note Do not use directly, use matrix_orbiting_y_up
template <typename T, bool RM, bool V>
class orbiting_y_up<matrix<T, 4, 4, RM, V>> {
public:
    constexpr orbiting_y_up(
      const vector<T, 3, V>& t,
      const vector<T, 3, V>& x,
      const vector<T, 3, V>& y,
      const vector<T, 3, V>& z,
      T r) noexcept
      : _t{t}
      , _x{x}
      , _z{z}
      , _y{y}
      , _r{r} {}

    constexpr orbiting_y_up(
      const vector<T, 3, V>& t,
      const T rs,
      const T sa,
      const T ca,
      const T se,
      const T ce) noexcept
      : _t{t}
      , _x{{-sa, T(0), -ca}}
      , _z{{ce * ca, se, ce * -sa}}
      , _y{cross(_z, _x)}
      , _r{rs} {}

    /// @brief Initalizes the matrix constructor.
    /// @param target is the point that the camera orbits around.
    /// @param radius is the distance from the target that the camera orbits at.
    /// @param azimuth is the azimuth (longitude) angle.
    /// @param elevation is the elevation (latitude) angle.
    constexpr orbiting_y_up(
      const vector<T, 3, V>& target,
      const T radius,
      const radians_t<T> azimuth,
      const radians_t<T> elevation)
      : orbiting_y_up{
          target,
          radius,
          sin(azimuth),
          cos(azimuth),
          sin(elevation),
          cos(elevation)} {}

    /// @brief Returns the constructed matrix.
    constexpr auto operator()() const noexcept {
        return _make(bool_constant<RM>());
    }

    friend constexpr auto reorder_mat_ctr(
      const orbiting_y_up<matrix<T, 4, 4, RM, V>>& c) noexcept
      -> orbiting_y_up<matrix<T, 4, 4, !RM, V>> {
        return {c._t, c._x, c._y, c._z, c._r};
    }

private:
    constexpr auto _make(const std::true_type) const noexcept {
        return matrix<T, 4, 4, true, V>{
          {{_x[0], _x[1], _x[2], -_r * dot(_x, _z) - dot(_x, _t)},
           {_y[0], _y[1], _y[2], -_r * dot(_y, _z) - dot(_y, _t)},
           {_z[0], _z[1], _z[2], -_r * dot(_z, _z) - dot(_z, _t)},
           {T(0), T(0), T(0), T(1)}}};
    }

    constexpr auto _make(const std::false_type) const noexcept {
        return reorder(_make(std::true_type()));
    }

    vector<T, 3, V> _t;
    vector<T, 3, V> _x, _z, _y;
    T _r;
};

/// @brief Alias for constructor of orbiting matrix used for camera transformations.
/// @ingroup math
template <typename T, bool V>
using matrix_orbiting_y_up =
  convertible_matrix_constructor<orbiting_y_up<matrix<T, 4, 4, true, V>>>;

} // namespace eagine::math

#endif // EAGINE_MATH_MATRIX_ORBITING_HPP
