
/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.types;
import eagine.core.math;
//------------------------------------------------------------------------------
// ray / plane intersection
//------------------------------------------------------------------------------
template <typename T, bool V>
void math_ray_plane_intersection_TV(eagitest::case_& test) {
    using ray = eagine::math::line<T, V>;
    using pln = eagine::math::plane<T, V>;
    using pnt = eagine::math::point<T, 3, V>;
    using vec = eagine::math::vector<T, 3, V>;

    test.check(
      not eagine::math::line_plane_intersection(
            ray{pnt{}, vec{T(2), T(0), T(0)}},
            pln{pnt{T(0), T(1), T(0)}, vec{T(0), T(-1), T(0)}})
            .has_value(),
      "A");

    test.check(
      eagine::math::line_plane_intersection(
        ray{pnt{T(-1), T(0), T(0)}, vec{T(2), T(0), T(0)}},
        pln{pnt{T(1), T(0), T(0)}, vec{T(-2), T(0), T(0)}})
        .value_or(pnt{})
        .close_to(pnt{T(1), T(0), T(0)}),
      "B");

    test.check(
      eagine::math::line_plane_intersection(
        ray{pnt{T(-2), T(0), T(0)}, vec{T(1), T(0), T(0)}},
        pln{pnt{T(2), T(0), T(0)}, vec{T(-2), T(0), T(0)}})
        .value_or(pnt{})
        .close_to(pnt{T(2), T(0), T(0)}),
      "C");

    test.check(
      eagine::math::line_plane_intersection(
        ray{pnt{T(0), T(2), T(0)}, vec{T(0), T(-4), T(0)}},
        pln{pnt{T(0), T(-3), T(0)}, vec{T(0), T(1), T(0)}})
        .value_or(pnt{})
        .close_to(pnt{T(0), T(-3), T(0)}),
      "D");
}
//------------------------------------------------------------------------------
template <typename T>
void math_ray_plane_intersection_T(eagitest::case_& test) {
    math_ray_plane_intersection_TV<T, true>(test);
    math_ray_plane_intersection_TV<T, false>(test);
}
//------------------------------------------------------------------------------
void math_ray_plane_intersection(auto& s) {
    eagitest::case_ test{s, 1, "ray / plane"};

    math_ray_plane_intersection_T<float>(test);
    math_ray_plane_intersection_T<double>(test);
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "intersection", 1};
    test.once(math_ray_plane_intersection);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
