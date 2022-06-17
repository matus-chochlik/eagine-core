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
import <utility>;
import <optional>;

namespace eagine::math {
//------------------------------------------------------------------------------
// utils
//------------------------------------------------------------------------------
export template <typename V>
constexpr auto nearest_ray_param(const std::pair<V, V>& params) noexcept -> V {

    const auto& [t0, t1] = params;

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
export template <typename T, bool V>
constexpr auto _line_sphere_intersection_a(
  const vector<T, 3, V> ld,
  const vector<T, 3, V> oc) noexcept -> T {
    return -T(2) * dot(ld, oc);
}
//------------------------------------------------------------------------------
export template <typename T, bool V>
constexpr auto _line_sphere_intersection_d(const vector<T, 3, V> ld) noexcept
  -> valid_if_positive<T> {
    return {T(2) * dot(ld, ld)};
}
//------------------------------------------------------------------------------
export template <typename T, bool V>
constexpr auto _line_sphere_intersection_b(
  const vector<T, 3, V> ld,
  const vector<T, 3, V> oc,
  const T sr) noexcept -> valid_if_positive<T> {
    using std::pow;
    return {
      pow(T(2) * dot(ld, oc), T(2)) -
      T(4) * dot(ld, ld) * (dot(oc, oc) - pow(sr, T(2)))};
}
//------------------------------------------------------------------------------
export template <typename T>
constexpr auto _line_sphere_intersection_t(
  const T a,
  const valid_if_positive<T> b,
  const valid_if_positive<T> d) noexcept {
    using std::sqrt;
    using E = std::optional<T>;
    using R = std::pair<E, E>;
    return d ? b ? R{E{(a + sqrt(extract(b))) / extract(d)},
                     E{(a - sqrt(extract(b))) / extract(d)}}
                 : R{E{a / extract(d)}, E{}}
             : R{};
}
//------------------------------------------------------------------------------
export template <typename T, bool V>
constexpr auto _line_sphere_intersection_p(
  const line<T, V>& ray,
  const std::pair<std::optional<T>, std::optional<T>>& ts) noexcept {
    using E = std::optional<vector<T, 3, V>>;
    using R = std::pair<E, E>;
    const auto& [t0, t1] = ts;
    return R{
      t0 ? E{ray.point_at(extract(t0))} : E{},
      t1 ? E{ray.point_at(extract(t1))} : E{}};
}
//------------------------------------------------------------------------------
export template <typename T, bool V>
auto line_sphere_intersection_params(
  const line<T, V>& ray,
  const sphere<T, V>& sph) noexcept
  -> std::pair<std::optional<T>, std::optional<T>> {
    const auto rdir{ray.direction()};
    const auto scro{ray.origin() - sph.center()};
    return _line_sphere_intersection_t<T>(
      _line_sphere_intersection_a(rdir, scro),
      _line_sphere_intersection_b(rdir, scro, sph.radius()),
      _line_sphere_intersection_d(rdir));
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
export template <typename T, bool V>
constexpr auto _line_sphere_intersection_n_p(
  const line<T, V>& ray,
  const std::pair<std::optional<T>, std::optional<T>>& ts) noexcept {
    using R = std::optional<vector<T, 3, V>>;
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
    if(const auto t{line_triangle_intersection_param(ray, tri)}) {
        return {ray.origin() + ray.direction() * extract(t)};
    }
    return {};
}
//------------------------------------------------------------------------------
} // namespace eagine::math

