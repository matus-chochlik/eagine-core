/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.math:primitives;

import std;
import eagine.core.types;
import :vector;

namespace eagine::math {
//------------------------------------------------------------------------------
/// @brief Template for planes in 3 dimensions.
/// @ingroup math
export template <typename T, bool V = true>
class plane {
public:
    /// @brief Default constructor.
    constexpr plane() noexcept = default;

    /// @brief Construction from point and normal.
    constexpr plane(const point<T, 3, V>& p, const vector<T, 3, V>& n) noexcept
      : _equation{n, -n.dot(p.to_vector())} {
        assert(not normal().is_zero());
    }

    /// @brief Construction from point and two vectors.
    constexpr plane(
      const point<T, 3, V>& p,
      const vector<T, 3, V>& u,
      const vector<T, 3, V>& v) noexcept
      : plane{cross(u, v).normalized(), p} {}

    /// @brief Construction from three points.
    constexpr plane(
      const point<T, 3, V>& p0,
      const point<T, 3, V>& p1,
      const point<T, 3, V>& p2) noexcept
      : plane{p0, p1 - p0, p2 - p0} {}

    /// @brief Returns the normal vector part of the plane equation.
    constexpr auto normal() const noexcept -> vector<T, 3, V> {
        return vector<T, 3, V>{_equation};
    }

    /// @brief Returns the distance part of the plane equation.
    constexpr auto distance() const noexcept -> T {
        return _equation[3] / normal().length();
    }

    /// @brief Returns the plane equation.
    constexpr auto equation() const noexcept -> const vector<T, 4, V>& {
        return _equation;
    }

private:
    vector<T, 4, V> _equation{};
};
//------------------------------------------------------------------------------
/// @brief Basic template for lines in N-dimensional space.
/// @ingroup math
export template <typename T, int N, bool V = true>
class basic_line {
public:
    /// @brief Default constructor.
    constexpr basic_line() noexcept = default;

    /// @brief Construction from origin point and direction vector.
    constexpr basic_line(
      const point<T, N, V> orig,
      const vector<T, N, V> dir) noexcept
      : _origin{orig}
      , _direction{dir} {}

    /// @brief Construction from a pair of points.
    constexpr basic_line(
      const point<T, N, V> bgn,
      const point<T, N, V> end) noexcept
      : _origin{bgn}
      , _direction{end - bgn} {}

    /// @brief Returns the line origin point.
    [[nodiscard]] constexpr auto origin() const noexcept -> point<T, N, V> {
        return _origin;
    }

    /// @brief Returns the line direction.
    [[nodiscard]] constexpr auto direction() const noexcept -> vector<T, N, V> {
        return _direction;
    }

    /// @brief Returns the line destination point.
    [[nodiscard]] constexpr auto destination() const noexcept
      -> point<T, N, V> {
        return _origin + _direction;
    }

    /// @brief Returns a point on the line at the specified parameter @p t.
    [[nodiscard]] constexpr auto point_at(T t) const noexcept
      -> point<T, N, V> {
        return _origin + _direction * t;
    }

private:
    point<T, N, V> _origin{};
    vector<T, N, V> _direction{};
};

/// @brief Alias for lines in 3D space.
/// @ingroup math
export template <typename T, bool V = true>
using line = basic_line<T, 3, V>;
//------------------------------------------------------------------------------
/// @brief Basic template for triangles in N-dimensional space.
/// @ingroup math
export template <typename T, int N, bool V = true>
class basic_triangle {
public:
    /// @brief Default constructor.
    constexpr basic_triangle() noexcept = default;

    /// @brief Construction from three points making up the triangle.
    constexpr basic_triangle(
      const point<T, N, V> a,
      const point<T, N, V> b,
      const point<T, N, V> c) noexcept
      : _vertices{{a, b, c}} {}

    /// @brief Returns the point at the specified index.
    /// @pre index < 3
    [[nodiscard]] constexpr auto vertex(const span_size_t index) const noexcept
      -> point<T, N, V> {
        return _vertices[index];
    }

    /// @brief Returns the first point of the triangle.
    [[nodiscard]] constexpr auto a() const noexcept -> point<T, N, V> {
        return _vertices[0];
    }

    /// @brief Returns the second point of the triangle.
    [[nodiscard]] constexpr auto b() const noexcept -> point<T, N, V> {
        return _vertices[1];
    }

    /// @brief Returns the third point of the triangle.
    [[nodiscard]] constexpr auto c() const noexcept -> point<T, N, V> {
        return _vertices[2];
    }

    /// @brief Returns the ab edge line of the triangle.
    [[nodiscard]] constexpr auto ab() const noexcept -> basic_line<T, N, V> {
        return {a(), b()};
    }

    /// @brief Returns the ac edge line of the triangle.
    [[nodiscard]] constexpr auto ac() const noexcept -> basic_line<T, N, V> {
        return {a(), c()};
    }

    /// @brief Returns the bc edge line of the triangle.
    [[nodiscard]] constexpr auto bc() const noexcept -> basic_line<T, N, V> {
        return {b(), c()};
    }

    /// @brief Returns the center of the triangle.
    [[nodiscard]] constexpr auto center() const noexcept -> point<T, N, V> {
        return ((a().to_vector() + b().to_vector() + c().to_vector()) / T(3))
          .to_point();
    }

    /// @brief Returns the normal vector of the triangle, in specified direction.
    [[nodiscard]] constexpr auto normal(bool cw) const noexcept
      -> vector<T, N, V> {
        return cw ? cross(ac().direction(), ab().direction())
                  : cross(ab().direction(), ac().direction());
    }

    /// @brief Returns the area of the triangle.
    [[nodiscard]] constexpr auto area() const noexcept -> T {
        return length(cross(ab().direction(), ac().direction())) / T(2);
    }

    /// @brief Returns the plane coplanar with this triangle.
    [[nodiscard]] constexpr auto to_plane() const noexcept -> plane<T, V>
        requires(N == 3)
    {
        return {a(), b(), c()};
    }

private:
    std::array<point<T, N, V>, 3> _vertices{};
};

/// @brief Alias for triangles in 3D space.
/// @ingroup math
export template <typename T, bool V = true>
using triangle = basic_triangle<T, 3, V>;
//------------------------------------------------------------------------------
/// @brief Basic template for spheres in N-dimensional space.
/// @ingroup math
export template <typename T, int N, bool V = true>
class basic_sphere {
public:
    /// @brief Default constructor.
    constexpr basic_sphere() noexcept = default;

    /// @brief Construction from center point and radius value.
    constexpr basic_sphere(const point<T, N, V> cntr, const T rad) noexcept
      : _params{cntr.to_vector(), rad} {}

    /// @brief Returns the sphere center as a vector.
    [[nodiscard]] constexpr auto direction() const noexcept -> vector<T, N, V> {
        return vector<T, N, V>{_params};
    }

    /// @brief Returns the sphere center.
    [[nodiscard]] constexpr auto center() const noexcept -> point<T, N, V> {
        return direction().to_point();
    }

    /// @brief Returns the sphere radius.
    [[nodiscard]] constexpr auto radius() const noexcept -> T {
        return _params[N];
    }

private:
    vector<T, N + 1, V> _params{};
};

/// @brief Alias for spheres in 3D space.
/// @ingroup math
export template <typename T, bool V = true>
using sphere = basic_sphere<T, 3, V>;
//------------------------------------------------------------------------------
} // namespace eagine::math
