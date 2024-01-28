/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.units;
import eagine.core.math;
//------------------------------------------------------------------------------
// spherical cartesian roundtrip
//------------------------------------------------------------------------------
template <typename T, bool V>
void math_coord_spherical_cartesian_roundtrip(eagitest::case_& test) {
    eagitest::track trck{test, 0, 2};
    auto& rg{test.random()};
    using namespace eagine::math;
    using eagine::radians_;
    using std::abs;

    for(unsigned i = 0; i < test.repeats(1000); ++i) {

        auto azim = radians_(rg.get_between(-T(pi), T(pi)));
        auto elev = radians_(rg.get_between(-T(pi) / 2, T(pi) / 2));

        unit_spherical_coordinate<T, V> orig(azim, elev);
        auto calc = to_unit_spherical(to_cartesian(orig));

        test.check_close(
          orig.elevation().value(), calc.elevation().value(), "elevation");
        const auto diff = (T(pi) / 2) - abs(elev.value());
        trck.checkpoint(1);
        if(diff > T(0)) {
            test.check_close(
              orig.azimuth().value(), calc.azimuth().value(), "azimuth");
            trck.checkpoint(2);
        }
    }
}
//------------------------------------------------------------------------------
void spherical_cartesian_roundtrip_float_true(auto& s) {
    eagitest::case_ test{s, 1, "spherical cartesian roundtrip float/true"};
    math_coord_spherical_cartesian_roundtrip<float, true>(test);
}
//------------------------------------------------------------------------------
void spherical_cartesian_roundtrip_float_false(auto& s) {
    eagitest::case_ test{s, 2, "spherical cartesian roundtrip float/false"};
    math_coord_spherical_cartesian_roundtrip<float, false>(test);
}
//------------------------------------------------------------------------------
void spherical_cartesian_roundtrip_double_true(auto& s) {
    eagitest::case_ test{s, 3, "spherical cartesian roundtrip double/true"};
    math_coord_spherical_cartesian_roundtrip<double, true>(test);
}
//------------------------------------------------------------------------------
void spherical_cartesian_roundtrip_double_false(auto& s) {
    eagitest::case_ test{s, 4, "spherical cartesian roundtrip double/false"};
    math_coord_spherical_cartesian_roundtrip<double, false>(test);
}
//------------------------------------------------------------------------------
// cartesian spherical roundtrip
//------------------------------------------------------------------------------
template <typename T, bool V>
void math_coord_cartesian_spherical_roundtrip(eagitest::case_& test) {
    auto& rg{test.random()};
    using namespace eagine::math;
    using std::abs;

    for(int i = 0; i < 1000; ++i) {

        auto x = rg.get_between(T(-1), T(1));
        auto y = rg.get_between(T(-1), T(1));
        auto z = rg.get_between(T(-1), T(1));

        const auto orig = normalized(vector<T, 3, V>::make(x, y, z));
        const auto calc = to_cartesian(to_unit_spherical(orig));

        test.check_close(orig.x(), calc.x(), "x");
        test.check_close(orig.y(), calc.y(), "y");
        test.check_close(orig.z(), calc.z(), "z");
    }
}
//------------------------------------------------------------------------------
void cartesian_spherical_roundtrip_float_true(auto& s) {
    eagitest::case_ test{s, 5, "spherical cartesian roundtrip float/true"};
    math_coord_cartesian_spherical_roundtrip<float, true>(test);
}
//------------------------------------------------------------------------------
void cartesian_spherical_roundtrip_float_false(auto& s) {
    eagitest::case_ test{s, 6, "spherical cartesian roundtrip float/false"};
    math_coord_cartesian_spherical_roundtrip<float, false>(test);
}
//------------------------------------------------------------------------------
void cartesian_spherical_roundtrip_double_true(auto& s) {
    eagitest::case_ test{s, 7, "spherical cartesian roundtrip double/true"};
    math_coord_cartesian_spherical_roundtrip<double, true>(test);
}
//------------------------------------------------------------------------------
void cartesian_spherical_roundtrip_double_false(auto& s) {
    eagitest::case_ test{s, 8, "spherical cartesian roundtrip double/false"};
    math_coord_cartesian_spherical_roundtrip<double, false>(test);
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "coordinates", 8};
    test.once(spherical_cartesian_roundtrip_float_true);
    test.once(spherical_cartesian_roundtrip_float_false);
    test.once(spherical_cartesian_roundtrip_double_true);
    test.once(spherical_cartesian_roundtrip_double_false);
    test.once(cartesian_spherical_roundtrip_float_true);
    test.once(cartesian_spherical_roundtrip_float_false);
    test.once(cartesian_spherical_roundtrip_double_true);
    test.once(cartesian_spherical_roundtrip_double_false);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
