/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.math:intersection;

import eagine.core.types;
import eagine.core.valid_if;
import :functions;
import :primitives;
import :vector;
import <cmath>;
import <optional>;
import <utility>;

namespace eagine::math {
//------------------------------------------------------------------------------
// utils
//------------------------------------------------------------------------------
export template <typename V>
constexpr auto nearest_ray_param(const std::pair<V, V>& params) noexcept -> V {

    const auto& t0 = std::get<0>(params);
    const auto& t1 = std::get<1>(params);

    if(t0 >= 0.F) {
        if(t1 >= 0.F) {
            if(t0 < t1) {
                return t0;
            } else {
                return t1;
            }
        } else {
            return t0;
        }
    } else {
        if(t1 >= 0.F) {
            return t1;
        }
    }
    return {};
}
//------------------------------------------------------------------------------
// line-sphere
//------------------------------------------------------------------------------
template <typename T, bool V>
constexpr auto _line_sphere_intersection_a(
  const vector<T, 3, V> ld,
  const vector<T, 3, V> oc) noexcept -> T {
    return -T(2) * dot(ld, oc);
}
//------------------------------------------------------------------------------
template <typename T, bool V>
constexpr auto _line_sphere_intersection_d(const vector<T, 3, V> ld) noexcept
  -> valid_if_positive<T> {
    return T(2) * dot(ld, ld);
}
//------------------------------------------------------------------------------
template <typename T, bool V>
constexpr auto _line_sphere_intersection_b(
  const vector<T, 3, V> ld,
  const vector<T, 3, V> oc,
  const T sr) noexcept -> valid_if_nonnegative<T> {
    using std::pow;
    return T(
      pow(2 * dot(ld, oc), 2) - 4 * dot(ld, ld) * (dot(oc, oc) - pow(sr, 2)));
}
//------------------------------------------------------------------------------
template <typename T>
constexpr auto _line_sphere_intersection_t(
  const T a,
  const valid_if_nonnegative<T> b,
  const valid_if_positive<T> d) noexcept {
    using std::sqrt;
    using R = std::pair<std::optional<T>, std::optional<T>>;
    return (b && d) ? (extract(b) > T(0))
                        ? R{{(a + sqrt(extract(b))) / extract(d)},
                            {(a - sqrt(extract(b))) / extract(d)}}
                        : R{{a / extract(d), true}, {}}
                    : R{};
}
//------------------------------------------------------------------------------
template <typename T, bool V>
constexpr auto _line_sphere_intersection_p(
  const line<T, V>& ray,
  const std::pair<std::optional<T>, std::optional<T>>& ts) noexcept {
    using E = optionally_valid<vector<T, 3, V>>;
    using R = std::pair<E, E>;
    return R{
      std::get<0>(ts) ? E{ray.point_at(extract(std::get<0>(ts)))} : E{},
      std::get<1>(ts) ? E{ray.point_at(extract(std::get<1>(ts)))} : E{},
    };
}
//------------------------------------------------------------------------------
export template <typename T, bool V>
auto line_sphere_intersection_params(
  const line<T, V>& ray,
  const sphere<T, V>& sph) noexcept
  -> std::pair<std::optional<T>, std::optional<T>> {
    return _line_sphere_intersection_t(
      _line_sphere_intersection_a(ray.direction(), ray.origin() - sph.center()),
      _line_sphere_intersection_b(
        ray.direction(), ray.origin() - sph.center(), sph.radius()),
      _line_sphere_intersection_d(ray.direction()));
}
//------------------------------------------------------------------------------
/// @brief Finds line-sphere intersection points.
/// @ingroup math
/// @see nearest_line_sphere_intersection
export template <typename T, bool V>
constexpr auto line_sphere_intersection(
  const line<T, V>& ray,
  const sphere<T, V>& sph) noexcept
  -> std::pair<std::optional<vector<T, 3, V>>, std::optional<vector<T, 3, V>>> {
    return _line_sphere_intersection_p(
      ray, line_sphere_intersection_params(ray, sph));
}
//------------------------------------------------------------------------------
template <typename T, bool V>
constexpr auto _line_sphere_intersection_n_p(
  const line<T, V>& ray,
  const std::pair<std::optional<T>, std::optional<T>>& ts) noexcept {
    using R = optionally_valid<vector<T, 3, V>>;
    using std::abs;

    return std::get<0>(ts)
             ? std::get<1>(ts)
                 ? abs(extract(std::get<0>(ts))) < abs(extract(std::get<1>(ts)))
                     ? R{ray.point_at(extract(std::get<0>(ts)))}
                     : R{ray.point_at(extract(std::get<1>(ts)))}
                 : R{ray.point_at(extract(std::get<0>(ts)))}
             : R{};
}
//------------------------------------------------------------------------------
/// @brief Finds nearest line-sphere intersection point.
/// @ingroup math
/// @see line_sphere_intersection
export template <typename T, bool V>
constexpr auto nearest_line_sphere_intersection(
  const line<T, V>& ray,
  const sphere<T, V>& sph) noexcept -> std::optional<vector<T, 3, V>> {
    return _line_sphere_intersection_n_p(
      ray, line_sphere_intersection_params(ray, sph));
}
//------------------------------------------------------------------------------
// line-triangle
//------------------------------------------------------------------------------
export template <typename T, bool V>
constexpr auto line_triangle_intersection_param(
  const line<T, V>& ray,
  const triangle<T, V>& tri) noexcept -> std::optional<T> {

    const vector<T, 3, V> h = cross(ray.direction(), tri.ac());
    const T a = dot(tri.ab(), h);

    if(const auto ia = reciprocal(a)) {
        const T f = extract(ia);
        const vector<T, 3, V> s = ray.origin() - tri.a();
        const T u = f * dot(s, h);

        if((u >= T(0)) && (u <= T(1))) {
            const vector<T, 3, V> q = cross(s, tri.ab());
            const T v = f * dot(ray.direction(), q);

            if((v >= T(0)) && (u + v <= T(1))) {
                const T t = f * dot(tri.ac(), q);
                if(t >= T(0)) {
                    return {t};
                }
            }
        }
    }
    return {};
}
//------------------------------------------------------------------------------
/// @brief Finds line-triangle intersection point.
/// @ingroup math
export template <typename T, bool V>
constexpr auto line_triangle_intersection(
  const line<T, V>& ray,
  const triangle<T, V>& tri) noexcept -> std::optional<vector<T, 3, V>> {
    if(const auto t = line_triangle_intersection_param(ray, tri)) {
        return {ray.origin() + ray.direction() * extract(t)};
    }
    return {};
}
//------------------------------------------------------------------------------
} // namespace eagine::math

