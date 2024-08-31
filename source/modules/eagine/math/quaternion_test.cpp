/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.math;
import eagine.core.units;
//------------------------------------------------------------------------------
// quaternion default constructor
//------------------------------------------------------------------------------
template <typename T, bool V>
void quaternion_default_ctr_TV(eagitest::case_& test) {
    eagine::math::quaternion<T, V> q{};
    test.check(q.is_zero(), "is zero");
    test.check(not q.is_unit(), "not is unit");
}
//------------------------------------------------------------------------------
template <typename T>
void quaternion_default_ctr_T(eagitest::case_& test) {
    quaternion_default_ctr_TV<T, true>(test);
    quaternion_default_ctr_TV<T, false>(test);
}
//------------------------------------------------------------------------------
void quaternion_default_ctr_float(auto& s) {
    eagitest::case_ test{s, 1, "default construct float"};
    quaternion_default_ctr_T<float>(test);
}
//------------------------------------------------------------------------------
void quaternion_default_ctr_double(auto& s) {
    eagitest::case_ test{s, 2, "default construct double"};
    quaternion_default_ctr_T<double>(test);
}
//------------------------------------------------------------------------------
// quaternion real+imag constructor
//------------------------------------------------------------------------------
template <typename T, bool V>
void quaternion_real_imag_ctr_TV(eagitest::case_& test) {
    const auto i{test.random().get_any<T>()};
    const auto j{test.random().get_any<T>()};
    const auto k{test.random().get_any<T>()};
    const auto r{test.random().get_any<T>()};
    const eagine::math::vector<T, 3, V> v{i, j, k};

    eagine::math::quaternion<T, V> q{r, v};
    test.check_equal(q.real(), r, "real");
    test.check_equal(q.imag().x(), i, "x");
    test.check_equal(q.imag().y(), j, "y");
    test.check_equal(q.imag().z(), k, "z");

    test.check_equal(q.r(), r, "r");
    test.check_equal(q.i(), i, "i");
    test.check_equal(q.j(), j, "j");
    test.check_equal(q.k(), k, "k");
}
//------------------------------------------------------------------------------
template <typename T>
void quaternion_real_imag_ctr_T(eagitest::case_& test) {
    quaternion_real_imag_ctr_TV<T, true>(test);
    quaternion_real_imag_ctr_TV<T, false>(test);
}
//------------------------------------------------------------------------------
void quaternion_real_imag_ctr_float(auto& s) {
    eagitest::case_ test{s, 3, "real+imag construct float"};
    quaternion_real_imag_ctr_T<float>(test);
}
//------------------------------------------------------------------------------
void quaternion_real_imag_ctr_double(auto& s) {
    eagitest::case_ test{s, 4, "real+imag construct double"};
    quaternion_real_imag_ctr_T<double>(test);
}
//------------------------------------------------------------------------------
// quaternion axis+angle constructor
//------------------------------------------------------------------------------
template <typename T, bool V>
void quaternion_axis_angle_ctr_TV(eagitest::case_& test) {
    const auto i{test.random().get_between<T>(-5000.F, 5000.F)};
    const auto j{test.random().get_between<T>(-5000.F, 5000.F)};
    const auto k{test.random().get_between<T>(-5000.F, 5000.F)};
    const auto a{test.random().get_between<T>(-5000.F, 5000.F)};

    const eagine::math::vector<T, 3, V> v{i, j, k};

    eagine::math::quaternion<T, V> q{v.normalized(), eagine::radians_t<T>{a}};
    test.check(not q.is_zero(), "not is zero");
    test.check(q.is_unit(), "not is unit");
}
//------------------------------------------------------------------------------
template <typename T>
void quaternion_axis_angle_ctr_T(eagitest::case_& test) {
    quaternion_axis_angle_ctr_TV<T, true>(test);
    quaternion_axis_angle_ctr_TV<T, false>(test);
}
//------------------------------------------------------------------------------
void quaternion_axis_angle_ctr_float(auto& s) {
    eagitest::case_ test{s, 5, "axis+angle construct float"};
    quaternion_axis_angle_ctr_T<float>(test);
}
//------------------------------------------------------------------------------
void quaternion_axis_angle_ctr_double(auto& s) {
    eagitest::case_ test{s, 6, "axis+angle construct double"};
    quaternion_axis_angle_ctr_T<double>(test);
}
//------------------------------------------------------------------------------
// quaternion conjugate
//------------------------------------------------------------------------------
template <typename T, bool V>
void quaternion_conjugate_TV(eagitest::case_& test) {
    const eagine::math::quaternion<T, V> p{
      eagine::math::vector<T, 3, V>{
        test.random().get_between<T>(-5000.F, 5000.F),
        test.random().get_between<T>(-5000.F, 5000.F),
        test.random().get_between<T>(-5000.F, 5000.F)}
        .normalized(),
      eagine::radians_t<T>{test.random().get_between<T>(-5000.F, 5000.F)}};

    const eagine::math::quaternion<T, V> c{p.conjugate()};

    test.check_equal(p.r(), c.r(), "r");
    test.check_equal(p.i(), -c.i(), "-i");
    test.check_equal(p.j(), -c.j(), "-j");
    test.check_equal(p.k(), -c.k(), "-k");
}
//------------------------------------------------------------------------------
template <typename T>
void quaternion_conjugate_T(eagitest::case_& test) {
    quaternion_conjugate_TV<T, true>(test);
    quaternion_conjugate_TV<T, false>(test);
}
//------------------------------------------------------------------------------
void quaternion_conjugate_float(auto& s) {
    eagitest::case_ test{s, 7, "conjugate float"};
    quaternion_conjugate_T<float>(test);
}
//------------------------------------------------------------------------------
void quaternion_conjugate_double(auto& s) {
    eagitest::case_ test{s, 8, "conjugate double"};
    quaternion_conjugate_T<double>(test);
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "quaternion", 8};
    test.once(quaternion_default_ctr_float);
    test.once(quaternion_default_ctr_double);
    test.once(quaternion_real_imag_ctr_float);
    test.once(quaternion_real_imag_ctr_double);
    test.once(quaternion_axis_angle_ctr_float);
    test.once(quaternion_axis_angle_ctr_double);
    test.once(quaternion_conjugate_float);
    test.once(quaternion_conjugate_double);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
