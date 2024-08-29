/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.math:primitives;

import std;
import eagine.core.types;
import :vector;

namespace eagine::math {
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
      const vector<T, N, V> a,
      const vector<T, N, V> b,
      const vector<T, N, V> c) noexcept
      : _vertices{{a, b, c}} {}

    /// @brief Returns the point at the specified index.
    /// @pre index < 3
    [[nodiscard]] constexpr auto vertex(const span_size_t index) const noexcept
      -> vector<T, N, V> {
        return _vertices[index];
    }

    /// @brief Returns the first point of the triangle.
    [[nodiscard]] constexpr auto a() const noexcept -> vector<T, N, V> {
        return _vertices[0];
    }

    /// @brief Returns the second point of the triangle.
    [[nodiscard]] constexpr auto b() const noexcept -> vector<T, N, V> {
        return _vertices[1];
    }

    /// @brief Returns the third point of the triangle.
    [[nodiscard]] constexpr auto c() const noexcept -> vector<T, N, V> {
        return _vertices[2];
    }

    /// @brief Returns the ab edge vector of the triangle.
    [[nodiscard]] constexpr auto ab() const noexcept -> vector<T, N, V> {
        return b() - a();
    }

    /// @brief Returns the ac edge vector of the triangle.
    [[nodiscard]] constexpr auto ac() const noexcept -> vector<T, N, V> {
        return c() - a();
    }

    /// @brief Returns the center of the triangle.
    [[nodiscard]] constexpr auto center() const noexcept -> vector<T, N, V> {
        return (a() + b() + c()) / T(3);
    }

    /// @brief Returns the normal vector of the triangle, in specified direction.
    [[nodiscard]] constexpr auto normal(bool cw) const noexcept
      -> vector<T, N, V> {
        return cw ? cross(ac(), ab()) : cross(ab(), ac());
    }

    /// @brief Returns the area of the triangle.
    [[nodiscard]] constexpr auto area() const noexcept -> T {
        return length(cross(ab(), ac())) / T(2);
    }

private:
    std::array<vector<T, N, V>, 3> _vertices{};
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
    constexpr basic_sphere(const vector<T, N, V> cntr, const T rad) noexcept
      : _params{cntr, rad} {}

    /// @brief Returns the sphere center.
    [[nodiscard]] constexpr auto center() const noexcept -> vector<T, N, V> {
        return {_params};
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
