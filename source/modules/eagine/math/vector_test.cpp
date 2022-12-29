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
template <typename T, int N, bool V>
void vector_default_ctr_TNV(eagitest::case_& test) {
    test.parameter(N, "N");
    eagine::math::vector<T, N, V> v{};
    for(int i = 0; i < N; ++i) {
        test.check_equal(v[i], T(0), "is zero");
    }
}
//------------------------------------------------------------------------------
template <typename T, bool V>
void vector_default_ctr_TV(eagitest::case_& test) {
    test.parameter(V, "V");
    vector_default_ctr_TNV<T, 1, V>(test);
    vector_default_ctr_TNV<T, 2, V>(test);
    vector_default_ctr_TNV<T, 3, V>(test);
    vector_default_ctr_TNV<T, 4, V>(test);
    vector_default_ctr_TNV<T, 5, V>(test);
    vector_default_ctr_TNV<T, 6, V>(test);
    vector_default_ctr_TNV<T, 7, V>(test);
    vector_default_ctr_TNV<T, 8, V>(test);
    vector_default_ctr_TNV<T, 11, V>(test);
    vector_default_ctr_TNV<T, 17, V>(test);
    vector_default_ctr_TNV<T, 23, V>(test);
}
//------------------------------------------------------------------------------
template <typename T>
void vector_default_ctr_T(eagitest::case_& test) {
    vector_default_ctr_TV<T, true>(test);
    vector_default_ctr_TV<T, false>(test);
}
//------------------------------------------------------------------------------
void vector_default_ctr_int(auto& s) {
    eagitest::case_ test{s, 1, "default construct int"};
    vector_default_ctr_T<int>(test);
}
//------------------------------------------------------------------------------
void vector_default_ctr_float(auto& s) {
    eagitest::case_ test{s, 2, "default construct float"};
    vector_default_ctr_T<float>(test);
}
//------------------------------------------------------------------------------
void vector_default_ctr_double(auto& s) {
    eagitest::case_ test{s, 3, "default construct double"};
    vector_default_ctr_T<double>(test);
}
//------------------------------------------------------------------------------
// zero
//------------------------------------------------------------------------------
template <typename T, int N, bool V>
void vector_zero_TNV(eagitest::case_& test) {
    test.parameter(N, "N");
    auto v = eagine::math::vector<T, N, V>::zero();
    for(int i = 0; i < N; ++i) {
        test.check_equal(v[i], T(0), "is zero");
    }
}
//------------------------------------------------------------------------------
template <typename T, bool V>
void vector_zero_TV(eagitest::case_& test) {
    test.parameter(V, "V");
    vector_zero_TNV<T, 1, V>(test);
    vector_zero_TNV<T, 2, V>(test);
    vector_zero_TNV<T, 3, V>(test);
    vector_zero_TNV<T, 4, V>(test);
    vector_zero_TNV<T, 5, V>(test);
    vector_zero_TNV<T, 6, V>(test);
    vector_zero_TNV<T, 7, V>(test);
    vector_zero_TNV<T, 8, V>(test);
    vector_zero_TNV<T, 11, V>(test);
    vector_zero_TNV<T, 17, V>(test);
    vector_zero_TNV<T, 23, V>(test);
}
//------------------------------------------------------------------------------
template <typename T>
void vector_zero_T(eagitest::case_& test) {
    vector_zero_TV<T, true>(test);
    vector_zero_TV<T, false>(test);
}
//------------------------------------------------------------------------------
void vector_zero_int(auto& s) {
    eagitest::case_ test{s, 4, "zero int"};
    vector_zero_T<int>(test);
}
//------------------------------------------------------------------------------
void vector_zero_float(auto& s) {
    eagitest::case_ test{s, 5, "zero float"};
    vector_zero_T<float>(test);
}
//------------------------------------------------------------------------------
void vector_zero_double(auto& s) {
    eagitest::case_ test{s, 6, "zero double"};
    vector_zero_T<double>(test);
}
//------------------------------------------------------------------------------
// fill
//------------------------------------------------------------------------------
template <typename T, int N, bool V>
void vector_fill_TNV(eagitest::case_& test) {
    test.parameter(N, "N");
    const auto r{test.random().get_any<T>()};
    auto v = eagine::math::vector<T, N, V>::fill(r);
    for(int i = 0; i < N; ++i) {
        test.check_equal(v[i], r, "correct value");
    }
}
//------------------------------------------------------------------------------
template <typename T, bool V>
void vector_fill_TV(eagitest::case_& test) {
    test.parameter(V, "V");
    vector_fill_TNV<T, 1, V>(test);
    vector_fill_TNV<T, 2, V>(test);
    vector_fill_TNV<T, 3, V>(test);
    vector_fill_TNV<T, 4, V>(test);
    vector_fill_TNV<T, 5, V>(test);
    vector_fill_TNV<T, 6, V>(test);
    vector_fill_TNV<T, 7, V>(test);
    vector_fill_TNV<T, 8, V>(test);
    vector_fill_TNV<T, 11, V>(test);
    vector_fill_TNV<T, 17, V>(test);
    vector_fill_TNV<T, 23, V>(test);
}
//------------------------------------------------------------------------------
template <typename T>
void vector_fill_T(eagitest::case_& test) {
    vector_fill_TV<T, true>(test);
    vector_fill_TV<T, false>(test);
}
//------------------------------------------------------------------------------
void vector_fill_int(auto& s) {
    eagitest::case_ test{s, 7, "fill int"};
    vector_fill_T<int>(test);
}
//------------------------------------------------------------------------------
void vector_fill_float(auto& s) {
    eagitest::case_ test{s, 8, "fill float"};
    vector_fill_T<float>(test);
}
//------------------------------------------------------------------------------
void vector_fill_double(auto& s) {
    eagitest::case_ test{s, 9, "fill double"};
    vector_fill_T<double>(test);
}
//------------------------------------------------------------------------------
// axis
//------------------------------------------------------------------------------
template <typename T, int N, bool V>
void vector_axis_TNV(eagitest::case_& test) {
    test.parameter(N, "N");
    for(int a = 0; a < N; ++a) {
        const auto r{test.random().get_any<T>()};
        auto v = eagine::math::vector<T, N, V>::axis(a, r);
        for(int i = 0; i < N; ++i) {
            test.check_equal(v[i], a == i ? r : T(0), "correct value");
        }
    }
}
//------------------------------------------------------------------------------
template <typename T, bool V>
void vector_axis_TV(eagitest::case_& test) {
    test.parameter(V, "V");
    vector_axis_TNV<T, 1, V>(test);
    vector_axis_TNV<T, 2, V>(test);
    vector_axis_TNV<T, 3, V>(test);
    vector_axis_TNV<T, 4, V>(test);
    vector_axis_TNV<T, 5, V>(test);
    vector_axis_TNV<T, 6, V>(test);
    vector_axis_TNV<T, 7, V>(test);
    vector_axis_TNV<T, 8, V>(test);
    vector_axis_TNV<T, 11, V>(test);
    vector_axis_TNV<T, 17, V>(test);
    vector_axis_TNV<T, 23, V>(test);
}
//------------------------------------------------------------------------------
template <typename T>
void vector_axis_T(eagitest::case_& test) {
    vector_axis_TV<T, true>(test);
    vector_axis_TV<T, false>(test);
}
//------------------------------------------------------------------------------
void vector_axis_int(auto& s) {
    eagitest::case_ test{s, 10, "axis int"};
    vector_axis_T<int>(test);
}
//------------------------------------------------------------------------------
void vector_axis_float(auto& s) {
    eagitest::case_ test{s, 11, "axis float"};
    vector_axis_T<float>(test);
}
//------------------------------------------------------------------------------
void vector_axis_double(auto& s) {
    eagitest::case_ test{s, 12, "axis double"};
    vector_axis_T<double>(test);
}
//------------------------------------------------------------------------------
// axis
//------------------------------------------------------------------------------
template <typename T, int I, int N, bool V>
void vector_axis_TINV(eagitest::case_& test) {
    test.parameter(N, "N");
    test.parameter(I, "I");
    const auto r{test.random().get_any<T>()};
    auto v = eagine::math::vector<T, N, V>::template axis<I>(r);
    for(int i = 0; i < N; ++i) {
        test.check_equal(v[i], I == i ? r : T(0), "correct value");
    }
}
//------------------------------------------------------------------------------
template <typename T, bool V>
void vector_axis_TIV(eagitest::case_& test) {
    test.parameter(V, "V");
    vector_axis_TINV<T, 0, 1, V>(test);

    vector_axis_TINV<T, 0, 2, V>(test);
    vector_axis_TINV<T, 1, 2, V>(test);

    vector_axis_TINV<T, 0, 3, V>(test);
    vector_axis_TINV<T, 1, 3, V>(test);
    vector_axis_TINV<T, 2, 3, V>(test);

    vector_axis_TINV<T, 0, 4, V>(test);
    vector_axis_TINV<T, 1, 4, V>(test);
    vector_axis_TINV<T, 2, 4, V>(test);
    vector_axis_TINV<T, 3, 4, V>(test);

    vector_axis_TINV<T, 0, 5, V>(test);
    vector_axis_TINV<T, 1, 5, V>(test);
    vector_axis_TINV<T, 2, 5, V>(test);
    vector_axis_TINV<T, 3, 5, V>(test);
    vector_axis_TINV<T, 4, 5, V>(test);

    vector_axis_TINV<T, 0, 6, V>(test);
    vector_axis_TINV<T, 1, 6, V>(test);
    vector_axis_TINV<T, 2, 6, V>(test);
    vector_axis_TINV<T, 3, 6, V>(test);
    vector_axis_TINV<T, 4, 6, V>(test);
    vector_axis_TINV<T, 5, 6, V>(test);

    vector_axis_TINV<T, 0, 7, V>(test);
    vector_axis_TINV<T, 1, 7, V>(test);
    vector_axis_TINV<T, 2, 7, V>(test);
    vector_axis_TINV<T, 3, 7, V>(test);
    vector_axis_TINV<T, 4, 7, V>(test);
    vector_axis_TINV<T, 5, 7, V>(test);
    vector_axis_TINV<T, 6, 7, V>(test);

    vector_axis_TINV<T, 0, 8, V>(test);
    vector_axis_TINV<T, 1, 8, V>(test);
    vector_axis_TINV<T, 2, 8, V>(test);
    vector_axis_TINV<T, 3, 8, V>(test);
    vector_axis_TINV<T, 4, 8, V>(test);
    vector_axis_TINV<T, 5, 8, V>(test);
    vector_axis_TINV<T, 6, 8, V>(test);
    vector_axis_TINV<T, 7, 8, V>(test);
}
//------------------------------------------------------------------------------
template <typename T>
void vector_axis_TI(eagitest::case_& test) {
    vector_axis_TIV<T, true>(test);
    vector_axis_TIV<T, false>(test);
}
//------------------------------------------------------------------------------
void vector_axis_I_int(auto& s) {
    eagitest::case_ test{s, 13, "axis I int"};
    vector_axis_TI<int>(test);
}
//------------------------------------------------------------------------------
void vector_axis_I_float(auto& s) {
    eagitest::case_ test{s, 14, "axis I float"};
    vector_axis_TI<float>(test);
}
//------------------------------------------------------------------------------
void vector_axis_I_double(auto& s) {
    eagitest::case_ test{s, 15, "axis I double"};
    vector_axis_TI<double>(test);
}
//------------------------------------------------------------------------------
// make
//------------------------------------------------------------------------------
template <typename T, bool V>
void vector_make_TV(eagitest::case_& test) {
    test.parameter(V, "V");
    auto& rg{test.random()};

    T r1 = rg.template get_any<T>();
    auto v1 = eagine::math::vector<T, 1, V>::make(r1);
    test.check_equal(v1[0], r1, "10");

    T r2 = rg.template get_any<T>();
    auto v2 = eagine::math::vector<T, 2, V>::make(r2, r1);
    test.check_equal(v2[0], r2, "20");
    test.check_equal(v2[1], r1, "21");

    T r3 = rg.template get_any<T>();
    auto v3 = eagine::math::vector<T, 3, V>::make(r3, r2, r1);
    test.check_equal(v3[0], r3, "30");
    test.check_equal(v3[1], r2, "31");
    test.check_equal(v3[2], r1, "32");

    T r4 = rg.template get_any<T>();
    auto v4 = eagine::math::vector<T, 4, V>::make(r4, r3, r2, r1);
    test.check_equal(v4[0], r4, "40");
    test.check_equal(v4[1], r3, "41");
    test.check_equal(v4[2], r2, "42");
    test.check_equal(v4[3], r1, "43");

    T r5 = rg.template get_any<T>();
    auto v5 = eagine::math::vector<T, 5, V>::make(r5, r4, r3, r2, r1);
    test.check_equal(v5[0], r5, "50");
    test.check_equal(v5[1], r4, "51");
    test.check_equal(v5[2], r3, "52");
    test.check_equal(v5[3], r2, "53");
    test.check_equal(v5[4], r1, "54");

    T r6 = rg.template get_any<T>();
    auto v6 = eagine::math::vector<T, 6, V>::make(r6, r5, r4, r3, r2, r1);
    test.check_equal(v6[0], r6, "60");
    test.check_equal(v6[1], r5, "61");
    test.check_equal(v6[2], r4, "62");
    test.check_equal(v6[3], r3, "63");
    test.check_equal(v6[4], r2, "64");
    test.check_equal(v6[5], r1, "65");

    T r7 = rg.template get_any<T>();
    auto v7 = eagine::math::vector<T, 7, V>::make(r7, r6, r5, r4, r3, r2, r1);
    test.check_equal(v7[0], r7, "70");
    test.check_equal(v7[1], r6, "71");
    test.check_equal(v7[2], r5, "72");
    test.check_equal(v7[3], r4, "73");
    test.check_equal(v7[4], r3, "74");
    test.check_equal(v7[5], r2, "75");
    test.check_equal(v7[6], r1, "76");

    T r8 = rg.template get_any<T>();
    auto v8 =
      eagine::math::vector<T, 8, V>::make(r8, r7, r6, r5, r4, r3, r2, r1);
    test.check_equal(v8[0], r8, "80");
    test.check_equal(v8[1], r7, "81");
    test.check_equal(v8[2], r6, "82");
    test.check_equal(v8[3], r5, "83");
    test.check_equal(v8[4], r4, "84");
    test.check_equal(v8[5], r3, "85");
    test.check_equal(v8[6], r2, "86");
    test.check_equal(v8[7], r1, "87");
}
//------------------------------------------------------------------------------
template <typename T>
void vector_make_T(eagitest::case_& test) {
    vector_make_TV<T, true>(test);
    vector_make_TV<T, false>(test);
}
//------------------------------------------------------------------------------
void vector_make_int(auto& s) {
    eagitest::case_ test{s, 16, "make int"};
    vector_make_T<int>(test);
}
//------------------------------------------------------------------------------
void vector_make_float(auto& s) {
    eagitest::case_ test{s, 17, "make float"};
    vector_make_T<float>(test);
}
//------------------------------------------------------------------------------
void vector_make_double(auto& s) {
    eagitest::case_ test{s, 18, "make double"};
    vector_make_T<double>(test);
}
//------------------------------------------------------------------------------
// from
//------------------------------------------------------------------------------
template <typename T, bool V>
void vector_from_TV(eagitest::case_& test) {
    test.parameter(V, "V");
    auto& rg{test.random()};

    T r1 = rg.template get_any<T>();
    auto v1 = eagine::math::vector<T, 1, V>::make(r1);
    test.check_equal(v1[0], r1, "10");

    T r2 = rg.template get_any<T>();
    auto v2 = eagine::math::vector<T, 2, V>::from(v1, r2);
    test.check_equal(v2[0], r1, "20");
    test.check_equal(v2[1], r2, "21");

    T r3 = rg.template get_any<T>();
    auto v3 = eagine::math::vector<T, 3, V>::from(v2, r3);
    test.check_equal(v3[0], r1, "30");
    test.check_equal(v3[1], r2, "31");
    test.check_equal(v3[2], r3, "32");

    T r4 = rg.template get_any<T>();
    auto v4 = eagine::math::vector<T, 4, V>::from(v3, r4);
    test.check_equal(v4[0], r1, "40");
    test.check_equal(v4[1], r2, "41");
    test.check_equal(v4[2], r3, "42");
    test.check_equal(v4[3], r4, "43");

    T r5 = rg.template get_any<T>();
    auto v5 = eagine::math::vector<T, 5, V>::from(v4, r5);
    test.check_equal(v5[0], r1, "50");
    test.check_equal(v5[1], r2, "51");
    test.check_equal(v5[2], r3, "52");
    test.check_equal(v5[3], r4, "53");
    test.check_equal(v5[4], r5, "54");

    T r6 = rg.template get_any<T>();
    auto v6 = eagine::math::vector<T, 6, V>::from(v5, r6);
    test.check_equal(v6[0], r1, "60");
    test.check_equal(v6[1], r2, "61");
    test.check_equal(v6[2], r3, "62");
    test.check_equal(v6[3], r4, "63");
    test.check_equal(v6[4], r5, "64");
    test.check_equal(v6[5], r6, "65");

    T r7 = rg.template get_any<T>();
    auto v7 = eagine::math::vector<T, 7, V>::from(v6, r7);
    test.check_equal(v7[0], r1, "70");
    test.check_equal(v7[1], r2, "71");
    test.check_equal(v7[2], r3, "72");
    test.check_equal(v7[3], r4, "73");
    test.check_equal(v7[4], r5, "74");
    test.check_equal(v7[5], r6, "75");
    test.check_equal(v7[6], r7, "76");

    T r8 = rg.template get_any<T>();
    auto v8 = eagine::math::vector<T, 8, V>::from(v7, r8);
    test.check_equal(v8[0], r1, "80");
    test.check_equal(v8[1], r2, "81");
    test.check_equal(v8[2], r3, "82");
    test.check_equal(v8[3], r4, "83");
    test.check_equal(v8[4], r5, "84");
    test.check_equal(v8[5], r6, "85");
    test.check_equal(v8[6], r7, "86");
    test.check_equal(v8[7], r8, "87");
}
//------------------------------------------------------------------------------
template <typename T>
void vector_from_T(eagitest::case_& test) {
    vector_from_TV<T, true>(test);
    vector_from_TV<T, false>(test);
}
//------------------------------------------------------------------------------
void vector_from_int(auto& s) {
    eagitest::case_ test{s, 19, "from int"};
    vector_from_T<int>(test);
}
//------------------------------------------------------------------------------
void vector_from_float(auto& s) {
    eagitest::case_ test{s, 20, "from float"};
    vector_from_T<float>(test);
}
//------------------------------------------------------------------------------
void vector_from_double(auto& s) {
    eagitest::case_ test{s, 21, "from double"};
    vector_from_T<double>(test);
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "anything", 21};
    test.once(vector_default_ctr_int);
    test.once(vector_default_ctr_float);
    test.once(vector_default_ctr_double);
    test.once(vector_zero_int);
    test.once(vector_zero_float);
    test.once(vector_zero_double);
    test.once(vector_fill_int);
    test.once(vector_fill_float);
    test.once(vector_fill_double);
    test.once(vector_axis_int);
    test.once(vector_axis_float);
    test.once(vector_axis_double);
    test.once(vector_axis_I_int);
    test.once(vector_axis_I_float);
    test.once(vector_axis_I_double);
    test.once(vector_make_int);
    test.once(vector_make_float);
    test.once(vector_make_double);
    test.once(vector_from_int);
    test.once(vector_from_float);
    test.once(vector_from_double);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
