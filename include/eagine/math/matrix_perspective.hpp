/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#ifndef EAGINE_MATH_MATRIX_PERSPECTIVE_HPP
#define EAGINE_MATH_MATRIX_PERSPECTIVE_HPP

#include "../quantities.hpp"
#include "matrix_ctr.hpp"

namespace eagine::math {

// perspective
template <typename X>
class perspective;

// is_matrix_constructor<perspective>
template <typename T, int N, bool RM, bool V>
struct is_matrix_constructor<perspective<matrix<T, N, N, RM, V>>>
  : std::true_type {};

/// @brief Implements constructor of perspective projection matrix.
/// @ingroup math
/// @see matrix_perspective
///
/// @note Do not use directly, use matrix_perspective.
template <typename T, bool RM, bool V>
class perspective<matrix<T, 4, 4, RM, V>> {
public:
    constexpr perspective(const vect::data_t<T, 6, V>& v) noexcept
      : _v{v} {}

    /// @brief Initialized the matrix constructor.
    constexpr perspective(
      const T x_left,
      const T x_right,
      const T y_bottom,
      const T y_top,
      const T z_near,
      const T z_far) noexcept
      : _v{x_left, x_right, y_bottom, y_top, z_near, z_far} {}

    /// @brief Returns the constructed matrix.
    constexpr auto operator()() const noexcept {
        return _make(bool_constant<RM>());
    }

    /// @brief Constructs perspective matrix with x-FOV angle and aspect ratio.
    static auto x(
      const radians_t<T> xfov,
      const T aspect,
      const T z_near,
      const T z_far) noexcept {
        EAGINE_ASSERT(aspect > T(0));
        EAGINE_ASSERT(T(xfov) > T(0));

        const T x_right = z_near * tan(xfov * T(0.5));
        const T x_left = -x_right;

        const T y_bottom = x_left / aspect;
        const T y_top = x_right / aspect;

        return perspective(x_left, x_right, y_bottom, y_top, z_near, z_far);
    }

    /// @brief Constructs perspective matrix with y-FOV angle and aspect ratio.
    static auto y(
      const radians_t<T> yfov,
      const T aspect,
      const T z_near,
      const T z_far) noexcept {
        EAGINE_ASSERT(aspect > T(0));
        EAGINE_ASSERT(T(yfov) > T(0));

        const T y_top = z_near * tan(yfov * T(0.5));
        const T y_bottom = -y_top;

        const T x_left = y_bottom * aspect;
        const T x_right = y_top * aspect;

        return perspective(x_left, x_right, y_bottom, y_top, z_near, z_far);
    }

    /// @brief Constructs perspective matrix with FOV angle and aspect ratio of 1.
    static auto square(
      const radians_t<T> fov,
      const T z_near,
      const T z_far) noexcept {
        EAGINE_ASSERT(T(fov) > T(0));

        const T x_right = z_near * tan(fov * T(0.5));
        const T x_left = -x_right;

        const T y_bottom = x_left;
        const T y_top = x_right;

        return perspective(x_left, x_right, y_bottom, y_top, z_near, z_far);
    }

private:
    constexpr auto _x_left() const noexcept {
        return _v[0];
    }

    constexpr auto _x_right() const noexcept {
        return _v[1];
    }

    constexpr auto _y_bottom() const noexcept {
        return _v[2];
    }

    constexpr auto _y_top() const noexcept {
        return _v[3];
    }

    constexpr auto _z_near() const noexcept {
        return _v[4];
    }

    constexpr auto _z_far() const noexcept {
        return _v[5];
    }

    constexpr auto _m00() const noexcept {
        return (T(2) * _z_near()) / (_x_right() - _x_left());
    }

    constexpr auto _m11() const noexcept {
        return (T(2) * _z_near()) / (_y_top() - _y_bottom());
    }

    constexpr auto _m22() const noexcept {
        return -(_z_far() + _z_near()) / (_z_far() - _z_near());
    }

    constexpr auto _m20() const noexcept {
        return (_x_right() + _x_left()) / (_x_right() - _x_left());
    }

    constexpr auto _m21() const noexcept {
        return (_y_top() + _y_bottom()) / (_y_top() - _y_bottom());
    }

    constexpr auto _m23() const noexcept {
        return -T(1);
    }

    constexpr auto _m32() const noexcept {
        return -(T(2) * _z_far() * _z_near()) / (_z_far() - _z_near());
    }

    constexpr auto _make(const std::true_type) const noexcept {
        return matrix<T, 4, 4, true, V>{
          {{_m00(), T(0), _m20(), T(0)},
           {T(0), _m11(), _m21(), T(0)},
           {T(0), T(0), _m22(), _m32()},
           {T(0), T(0), _m23(), T(0)}}};
    }

    constexpr auto _make(const std::false_type) const noexcept {
        return matrix<T, 4, 4, false, V>{
          {{_m00(), T(0), T(0), T(0)},
           {T(0), _m11(), T(0), T(0)},
           {_m20(), _m21(), _m22(), _m23()},
           {T(0), T(0), _m32(), T(0)}}};
    }

    using _dT = vect::data_t<T, 6, V>;
    _dT _v;
};

// reorder_mat_ctr(perspective)
template <typename T, int N, bool RM, bool V>
static constexpr auto reorder_mat_ctr(
  const perspective<matrix<T, N, N, RM, V>>& c) noexcept
  -> perspective<matrix<T, N, N, !RM, V>> {
    return {c._v};
}

/// @brief Alias for constructor of perspective projection matrix.
/// @ingroup math
template <typename T, bool V>
using matrix_perspective =
  convertible_matrix_constructor<perspective<matrix<T, 4, 4, true, V>>>;

} // namespace eagine::math

#endif // EAGINE_MATH_MATRIX_PERSPECTIVE_HPP
