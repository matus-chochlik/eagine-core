/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.math;
//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
template <typename T, bool V>
void scalar_default_ctr_TV(eagitest::case_& test) {
    test.parameter(V, "V");
    eagine::math::scalar<T, 1, V> s1;
    test.check_equal(s1, T(0), "s1");
    eagine::math::scalar<T, 2, V> s2;
    test.check_equal(s2, T(0), "s2");
    eagine::math::scalar<T, 3, V> s3;
    test.check_equal(s3, T(0), "s3");
    eagine::math::scalar<T, 4, V> s4;
    test.check_equal(s4, T(0), "s4");
    eagine::math::scalar<T, 5, V> s5;
    test.check_equal(s5, T(0), "s5");
    eagine::math::scalar<T, 6, V> s6;
    test.check_equal(s6, T(0), "s6");
    eagine::math::scalar<T, 7, V> s7;
    test.check_equal(s7, T(0), "s7");
    eagine::math::scalar<T, 8, V> s8;
    test.check_equal(s8, T(0), "s8");
    eagine::math::scalar<T, 11, V> s11;
    test.check_equal(s11, T(0), "s11");
    eagine::math::scalar<T, 17, V> s17;
    test.check_equal(s17, T(0), "s17");
    eagine::math::scalar<T, 23, V> s23;
    test.check_equal(s23, T(0), "s23");
}
//------------------------------------------------------------------------------
template <typename T>
void scalar_default_ctr_T(eagitest::case_& test) {
    scalar_default_ctr_TV<T, true>(test);
    scalar_default_ctr_TV<T, false>(test);
}
//------------------------------------------------------------------------------
void scalar_default_construct_int(auto& s) {
    eagitest::case_ test{s, 1, "default construct int"};
    scalar_default_ctr_T<int>(test);
}
//------------------------------------------------------------------------------
void scalar_default_construct_float(auto& s) {
    eagitest::case_ test{s, 2, "default construct float"};
    scalar_default_ctr_T<float>(test);
}
//------------------------------------------------------------------------------
void scalar_default_construct_double(auto& s) {
    eagitest::case_ test{s, 3, "default construct double"};
    scalar_default_ctr_T<double>(test);
}
//------------------------------------------------------------------------------
// copy
//------------------------------------------------------------------------------
template <typename T, bool V>
void scalar_make_and_conv_TV(eagitest::case_& test) {
    auto s1 = eagine::math::scalar<T, 1, V>::make(1);
    T s1v{s1};
    test.check_equal(s1v, T(1), "1");

    auto s2 = eagine::math::scalar<T, 2, V>::make(2);
    T s2v = s2;
    test.check_equal(s2v, T(2), "2");

    auto s3 = eagine::math::scalar<T, 3, V>::make(3);
    T s3v{s3};
    test.check_equal(s3v, T(3), "3");

    auto s4 = eagine::math::scalar<T, 4, V>::make(4);
    T s4v = s4;
    test.check_equal(s4v, T(4), "4");

    auto s5 = eagine::math::scalar<T, 5, V>::make(5);
    T s5v{s5};
    test.check_equal(s5v, T(5), "5");

    auto s6 = eagine::math::scalar<T, 6, V>::make(6);
    T s6v = s6;
    test.check_equal(s6v, T(6), "6");

    auto s7 = eagine::math::scalar<T, 7, V>::make(7);
    T s7v{s7};
    test.check_equal(s7v, T(7), "7");

    auto s8 = eagine::math::scalar<T, 8, V>::make(8);
    T s8v = s8;
    test.check_equal(s8v, T(8), "8");

    auto s11 = eagine::math::scalar<T, 11, V>::make(11);
    T s11v{s11};
    test.check_equal(s11v, T(11), "11");

    auto s17 = eagine::math::scalar<T, 17, V>::make(17);
    T s17v = s17;
    test.check_equal(s17v, T(17), "17");

    auto s23 = eagine::math::scalar<T, 23, V>::make(23);
    T s23v{s23};
    test.check_equal(s23v, T(23), "23");
}
//------------------------------------------------------------------------------
template <typename T>
void scalar_make_and_conv_T(eagitest::case_& test) {
    scalar_make_and_conv_TV<T, true>(test);
    scalar_make_and_conv_TV<T, false>(test);
}
//------------------------------------------------------------------------------
void scalar_make_and_conv_int(auto& s) {
    eagitest::case_ test{s, 4, "make, copy, convert int"};
    scalar_make_and_conv_T<int>(test);
}
//------------------------------------------------------------------------------
void scalar_make_and_conv_float(auto& s) {
    eagitest::case_ test{s, 5, "make, copy, convert float"};
    scalar_make_and_conv_T<float>(test);
}
//------------------------------------------------------------------------------
void scalar_make_and_conv_double(auto& s) {
    eagitest::case_ test{s, 6, "make, copy, convert double"};
    scalar_make_and_conv_T<double>(test);
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "scalar", 6};
    test.once(scalar_default_construct_int);
    test.once(scalar_default_construct_float);
    test.once(scalar_default_construct_double);
    test.once(scalar_make_and_conv_int);
    test.once(scalar_make_and_conv_float);
    test.once(scalar_make_and_conv_double);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
