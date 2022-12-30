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
// vector default constructor
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
// from 2
//------------------------------------------------------------------------------
template <typename T, bool V>
void vector_from2_TV(eagitest::case_& test) {
    test.parameter(V, "V");
    auto& rg{test.random()};

    T ra = rg.get_any<T>();
    auto v1a = eagine::math::vector<T, 1, V>::make(ra);
    test.check_equal(v1a[0], ra, "a0");

    T rb = rg.get_any<T>();
    auto v1b = eagine::math::vector<T, 1, V>::make(rb);
    test.check_equal(v1b[0], rb, "b0");

    auto v2aa = eagine::math::vector<T, 2, V>::from(v1a, v1a);
    test.check_equal(v2aa[0], ra, "aa0");
    test.check_equal(v2aa[1], ra, "aa1");

    auto v2ab = eagine::math::vector<T, 2, V>::from(v1a, v1b);
    test.check_equal(v2ab[0], ra, "ab0");
    test.check_equal(v2ab[1], rb, "ab1");

    auto v2bb = eagine::math::vector<T, 2, V>::from(v1b, v1b);
    test.check_equal(v2bb[0], rb, "bb0");
    test.check_equal(v2bb[1], rb, "bb1");

    auto v3aba = eagine::math::vector<T, 3, V>::from(v2ab, v1a);
    test.check_equal(v3aba[0], ra, "aba0");
    test.check_equal(v3aba[1], rb, "aba1");
    test.check_equal(v3aba[2], ra, "aba2");

    auto v3bab = eagine::math::vector<T, 3, V>::from(v1b, v2ab);
    test.check_equal(v3bab[0], rb, "bab0");
    test.check_equal(v3bab[1], ra, "bab1");
    test.check_equal(v3bab[2], rb, "bab2");

    auto v4abab = eagine::math::vector<T, 4, V>::from(v1a, v3bab);
    test.check_equal(v4abab[0], ra, "abab0");
    test.check_equal(v4abab[1], rb, "abab1");
    test.check_equal(v4abab[2], ra, "abab2");
    test.check_equal(v4abab[3], rb, "abab3");

    auto v4aabb = eagine::math::vector<T, 4, V>::from(v2aa, v2bb);
    test.check_equal(v4aabb[0], ra, "aabb0");
    test.check_equal(v4aabb[1], ra, "aabb1");
    test.check_equal(v4aabb[2], rb, "aabb2");
    test.check_equal(v4aabb[3], rb, "aabb3");

    auto v4baba = eagine::math::vector<T, 4, V>::from(v3bab, v1a);
    test.check_equal(v4baba[0], rb, "baba0");
    test.check_equal(v4baba[1], ra, "baba1");
    test.check_equal(v4baba[2], rb, "baba2");
    test.check_equal(v4baba[3], ra, "baba3");

    auto v5ababa = eagine::math::vector<T, 5, V>::from(v1a, v4baba);
    test.check_equal(v5ababa[0], ra, "ababa0");
    test.check_equal(v5ababa[1], rb, "ababa1");
    test.check_equal(v5ababa[2], ra, "ababa2");
    test.check_equal(v5ababa[3], rb, "ababa3");
    test.check_equal(v5ababa[4], ra, "ababa4");

    auto v5aabab = eagine::math::vector<T, 5, V>::from(v2aa, v3bab);
    test.check_equal(v5aabab[0], ra, "aabab0");
    test.check_equal(v5aabab[1], ra, "aabab1");
    test.check_equal(v5aabab[2], rb, "aabab2");
    test.check_equal(v5aabab[3], ra, "aabab3");
    test.check_equal(v5aabab[4], rb, "aabab4");

    auto v5ababb = eagine::math::vector<T, 5, V>::from(v3aba, v2bb);
    test.check_equal(v5ababb[0], ra, "ababb0");
    test.check_equal(v5ababb[1], rb, "ababb1");
    test.check_equal(v5ababb[2], ra, "ababb2");
    test.check_equal(v5ababb[3], rb, "ababb3");
    test.check_equal(v5ababb[4], rb, "ababb4");

    auto v5babab = eagine::math::vector<T, 5, V>::from(v4baba, v1b);
    test.check_equal(v5babab[0], rb, "babab0");
    test.check_equal(v5babab[1], ra, "babab1");
    test.check_equal(v5babab[2], rb, "babab2");
    test.check_equal(v5babab[3], ra, "babab3");
    test.check_equal(v5babab[4], rb, "babab4");
}
//------------------------------------------------------------------------------
template <typename T>
void vector_from2_T(eagitest::case_& test) {
    vector_from2_TV<T, true>(test);
    vector_from2_TV<T, false>(test);
}
//------------------------------------------------------------------------------
void vector_from2_int(auto& s) {
    eagitest::case_ test{s, 22, "from 2 int"};
    vector_from2_T<int>(test);
}
//------------------------------------------------------------------------------
void vector_from2_float(auto& s) {
    eagitest::case_ test{s, 23, "from 2 float"};
    vector_from2_T<float>(test);
}
//------------------------------------------------------------------------------
void vector_from2_double(auto& s) {
    eagitest::case_ test{s, 24, "from 2 double"};
    vector_from2_T<double>(test);
}
//------------------------------------------------------------------------------
// from 3
//------------------------------------------------------------------------------
template <typename T, int N, bool V>
void vector_from3_TNV(eagitest::case_& test) {
    test.parameter(N, "N");
    test.parameter(V, "V");
    auto& rg{test.random()};

    T a[N];
    for(int i = 0; i < N; ++i) {
        a[i] = rg.get_any<T>();
    }

    auto v = eagine::math::vector<T, N, V>::from(a, N);

    for(int i = 0; i < N; ++i) {
        test.check_equal(v[i], a[i], "equal");
    }
}
//------------------------------------------------------------------------------
template <typename T, bool V>
void vector_from3_TV(eagitest::case_& test) {
    vector_from3_TNV<T, 1, V>(test);
    vector_from3_TNV<T, 2, V>(test);
    vector_from3_TNV<T, 3, V>(test);
    vector_from3_TNV<T, 4, V>(test);
    vector_from3_TNV<T, 5, V>(test);
    vector_from3_TNV<T, 6, V>(test);
    vector_from3_TNV<T, 7, V>(test);
    vector_from3_TNV<T, 8, V>(test);
    vector_from3_TNV<T, 12, V>(test);
    vector_from3_TNV<T, 15, V>(test);
    vector_from3_TNV<T, 20, V>(test);
}
//------------------------------------------------------------------------------
template <typename T>
void vector_from3_T(eagitest::case_& test) {
    vector_from3_TV<T, true>(test);
    vector_from3_TV<T, false>(test);
}
//------------------------------------------------------------------------------
void vector_from3_int(auto& s) {
    eagitest::case_ test{s, 25, "from 3 int"};
    vector_from3_T<int>(test);
}
//------------------------------------------------------------------------------
void vector_from3_float(auto& s) {
    eagitest::case_ test{s, 26, "from 3 float"};
    vector_from3_T<float>(test);
}
//------------------------------------------------------------------------------
void vector_from3_double(auto& s) {
    eagitest::case_ test{s, 27, "from 3 double"};
    vector_from3_T<double>(test);
}
//------------------------------------------------------------------------------
// from 4
//------------------------------------------------------------------------------
template <typename T, int N, bool V>
void vector_from4_TNV(eagitest::case_& test) {
    test.parameter(N, "N");
    test.parameter(V, "V");
    auto& rg{test.random()};

    for(int j = 0; j < N; ++j) {
        T a[N];
        for(int i = 0; i < N - j; ++i) {
            a[i] = rg.get_any<T>();
        }

        T r = rg.get_any<T>();

        auto v = eagine::math::vector<T, N, V>::from(a, N - j, r);

        for(int i = 0; i < N - j; ++i) {
            test.check_equal(v[i], a[i], "equal 1");
        }

        for(int i = N - j; i < N; ++i) {
            test.check_equal(v[i], r, "equal 2");
        }
    }
}
//------------------------------------------------------------------------------
template <typename T, bool V>
void vector_from4_TV(eagitest::case_& test) {
    vector_from4_TNV<T, 1, V>(test);
    vector_from4_TNV<T, 2, V>(test);
    vector_from4_TNV<T, 3, V>(test);
    vector_from4_TNV<T, 4, V>(test);
    vector_from4_TNV<T, 5, V>(test);
    vector_from4_TNV<T, 6, V>(test);
    vector_from4_TNV<T, 7, V>(test);
    vector_from4_TNV<T, 8, V>(test);
    vector_from4_TNV<T, 12, V>(test);
    vector_from4_TNV<T, 15, V>(test);
    vector_from4_TNV<T, 20, V>(test);
}
//------------------------------------------------------------------------------
template <typename T>
void vector_from4_T(eagitest::case_& test) {
    vector_from4_TV<T, true>(test);
    vector_from4_TV<T, false>(test);
}
//------------------------------------------------------------------------------
void vector_from4_int(auto& s) {
    eagitest::case_ test{s, 28, "from 4 int"};
    vector_from4_T<int>(test);
}
//------------------------------------------------------------------------------
void vector_from4_float(auto& s) {
    eagitest::case_ test{s, 29, "from 4 float"};
    vector_from4_T<float>(test);
}
//------------------------------------------------------------------------------
void vector_from4_double(auto& s) {
    eagitest::case_ test{s, 30, "from 4 double"};
    vector_from4_T<double>(test);
}
//------------------------------------------------------------------------------
// vector dimension
//------------------------------------------------------------------------------
template <typename T, int N, bool V>
void vector_dimension_TNV(eagitest::case_& test) {
    test.parameter(N, "N");
    test.parameter(V, "V");
    eagine::math::vector<T, N, V> v;
    test.check_equal(dimension(v), N, "dimension");
}
//------------------------------------------------------------------------------
template <typename T, bool V>
void vector_dimension_TV(eagitest::case_& test) {
    vector_dimension_TNV<T, 1, V>(test);
    vector_dimension_TNV<T, 2, V>(test);
    vector_dimension_TNV<T, 3, V>(test);
    vector_dimension_TNV<T, 4, V>(test);
    vector_dimension_TNV<T, 5, V>(test);
    vector_dimension_TNV<T, 6, V>(test);
    vector_dimension_TNV<T, 7, V>(test);
    vector_dimension_TNV<T, 8, V>(test);
    vector_dimension_TNV<T, 13, V>(test);
    vector_dimension_TNV<T, 17, V>(test);
}
//------------------------------------------------------------------------------
template <typename T>
void vector_dimension_T(eagitest::case_& test) {
    vector_dimension_TV<T, true>(test);
    vector_dimension_TV<T, false>(test);
}
//------------------------------------------------------------------------------
void vector_dimension(auto& s) {
    eagitest::case_ test{s, 31, "dimension"};
    vector_dimension_T<int>(test);
    vector_dimension_T<float>(test);
    vector_dimension_T<double>(test);
}
//------------------------------------------------------------------------------
// magnitude
//------------------------------------------------------------------------------
template <typename T, int N, bool V>
void vector_magnitude_TNV(eagitest::case_& test) {
    test.parameter(N, "N");
    test.parameter(V, "V");
    auto& rg{test.random()};

    T a[N];
    T m = T(0);

    for(int i = 0; i < N; ++i) {
        a[i] = rg.get_between<T>(-5000, 5000);
        m += a[i] * a[i];
    }

    auto v = eagine::math::vector<T, N, V>::from(a, N);

    using std::sqrt;
    test.check_close(T(magnitude(v)), T(sqrt(m)), "magnitude");
}
//------------------------------------------------------------------------------
template <typename T, bool V>
void vector_magnitude_TV(eagitest::case_& test) {
    vector_magnitude_TNV<T, 1, V>(test);
    vector_magnitude_TNV<T, 2, V>(test);
    vector_magnitude_TNV<T, 3, V>(test);
    vector_magnitude_TNV<T, 4, V>(test);
    vector_magnitude_TNV<T, 5, V>(test);
    vector_magnitude_TNV<T, 6, V>(test);
    vector_magnitude_TNV<T, 7, V>(test);
    vector_magnitude_TNV<T, 8, V>(test);
    vector_magnitude_TNV<T, 13, V>(test);
    vector_magnitude_TNV<T, 17, V>(test);
}
//------------------------------------------------------------------------------
template <typename T>
void vector_magnitude_T(eagitest::case_& test) {
    vector_magnitude_TV<T, true>(test);
    vector_magnitude_TV<T, false>(test);
}
//------------------------------------------------------------------------------
void vector_magnitude(auto& s) {
    eagitest::case_ test{s, 32, "magnitude"};
    vector_magnitude_T<int>(test);
    vector_magnitude_T<float>(test);
    vector_magnitude_T<double>(test);
}
//------------------------------------------------------------------------------
// dot product
//------------------------------------------------------------------------------
template <typename T, int N, bool V>
void vector_dot_TNV(eagitest::case_& test) {
    test.parameter(N, "N");
    test.parameter(V, "V");
    auto& rg{test.random()};

    T a[N], b[N];
    T d = T(0);

    for(int i = 0; i < N; ++i) {
        a[i] = rg.get_between<T>(-5000, 5000);
        b[i] = rg.get_between<T>(-5000, 5000);
        d += a[i] * b[i];
    }

    auto u = eagine::math::vector<T, N, V>::from(a, N);
    auto v = eagine::math::vector<T, N, V>::from(b, N);

    test.check_close(T(dot(u, v)), d, "dot 1");
    test.check_close(T(dot(u, v)), T(dot(v, u)), "dot 2");
}
//------------------------------------------------------------------------------
template <typename T, bool V>
void vector_dot_TV(eagitest::case_& test) {
    vector_dot_TNV<T, 1, V>(test);
    vector_dot_TNV<T, 2, V>(test);
    vector_dot_TNV<T, 3, V>(test);
    vector_dot_TNV<T, 4, V>(test);
    vector_dot_TNV<T, 5, V>(test);
    vector_dot_TNV<T, 6, V>(test);
    vector_dot_TNV<T, 7, V>(test);
    vector_dot_TNV<T, 8, V>(test);
    vector_dot_TNV<T, 13, V>(test);
    vector_dot_TNV<T, 17, V>(test);
}
//------------------------------------------------------------------------------
template <typename T>
void vector_dot_T(eagitest::case_& test) {
    vector_dot_TV<T, true>(test);
    vector_dot_TV<T, false>(test);
}
//------------------------------------------------------------------------------
void vector_dot(auto& s) {
    eagitest::case_ test{s, 33, "dot"};
    vector_dot_T<int>(test);
    vector_dot_T<float>(test);
    vector_dot_T<double>(test);
}
//------------------------------------------------------------------------------
// tvec default constructor
//------------------------------------------------------------------------------
template <typename T, int N, bool V>
void tvec_default_ctr_TNV(eagitest::case_& test) {
    test.parameter(N, "N");
    test.parameter(V, "V");
    eagine::math::tvec<T, N, V> v;
    for(int i = 0; i < N; ++i) {
        test.check_equal(v[i], T(0), "is zero");
    }
}
//------------------------------------------------------------------------------
template <typename T, bool V>
void tvec_default_ctr_TV(eagitest::case_& test) {
    tvec_default_ctr_TNV<T, 1, V>(test);
    tvec_default_ctr_TNV<T, 2, V>(test);
    tvec_default_ctr_TNV<T, 3, V>(test);
    tvec_default_ctr_TNV<T, 4, V>(test);
    tvec_default_ctr_TNV<T, 5, V>(test);
    tvec_default_ctr_TNV<T, 6, V>(test);
    tvec_default_ctr_TNV<T, 7, V>(test);
    tvec_default_ctr_TNV<T, 8, V>(test);
    tvec_default_ctr_TNV<T, 11, V>(test);
    tvec_default_ctr_TNV<T, 13, V>(test);
    tvec_default_ctr_TNV<T, 19, V>(test);
    tvec_default_ctr_TNV<T, 23, V>(test);
}
//------------------------------------------------------------------------------
template <typename T>
void tvec_default_ctr_T(eagitest::case_& test) {
    tvec_default_ctr_TV<T, true>(test);
    tvec_default_ctr_TV<T, false>(test);
}
//------------------------------------------------------------------------------
void tvec_default_ctr(auto& s) {
    eagitest::case_ test{s, 34, "tvec default constructor"};
    tvec_default_ctr_T<int>(test);
    tvec_default_ctr_T<float>(test);
    tvec_default_ctr_T<double>(test);
}
//------------------------------------------------------------------------------
// tvec fill
//------------------------------------------------------------------------------
template <typename T, int N, bool V>
void tvec_fill_ctr_TNV(eagitest::case_& test) {
    test.parameter(N, "N");
    test.parameter(V, "V");
    auto& rg{test.random()};

    const T r = rg.get_any<T>();
    eagine::math::tvec<T, N, V> v(r);
    for(int i = 0; i < N; ++i) {
        test.check_equal(v[i], r, "value");
    }
}
//------------------------------------------------------------------------------
template <typename T, bool V>
void tvec_fill_ctr_TV(eagitest::case_& test) {
    tvec_fill_ctr_TNV<T, 1, V>(test);
    tvec_fill_ctr_TNV<T, 2, V>(test);
    tvec_fill_ctr_TNV<T, 3, V>(test);
    tvec_fill_ctr_TNV<T, 4, V>(test);
    tvec_fill_ctr_TNV<T, 5, V>(test);
    tvec_fill_ctr_TNV<T, 6, V>(test);
    tvec_fill_ctr_TNV<T, 7, V>(test);
    tvec_fill_ctr_TNV<T, 8, V>(test);
    tvec_fill_ctr_TNV<T, 11, V>(test);
    tvec_fill_ctr_TNV<T, 13, V>(test);
    tvec_fill_ctr_TNV<T, 19, V>(test);
    tvec_fill_ctr_TNV<T, 23, V>(test);
}
//------------------------------------------------------------------------------
template <typename T>
void tvec_fill_ctr_T(eagitest::case_& test) {
    tvec_fill_ctr_TV<T, true>(test);
    tvec_fill_ctr_TV<T, false>(test);
}
//------------------------------------------------------------------------------
void tvec_fill_ctr(auto& s) {
    eagitest::case_ test{s, 35, "tvec fill constructor"};
    tvec_fill_ctr_T<int>(test);
    tvec_fill_ctr_T<float>(test);
    tvec_fill_ctr_T<double>(test);
}
//------------------------------------------------------------------------------
// tvec vector constructor
//------------------------------------------------------------------------------
template <typename T, int N, bool V>
void tvec_vector_ctr_TNV(eagitest::case_& test) {
    test.parameter(N, "N");
    test.parameter(V, "V");
    auto& rg{test.random()};

    const T r = rg.get_any<T>();
    eagine::math::tvec<T, N, V> v{eagine::math::vector<T, N, V>::fill(r)};
    for(int i = 0; i < N; ++i) {
        test.check_equal(v[i], r, "equal");
    }
}
//------------------------------------------------------------------------------
template <typename T, bool V>
void tvec_vector_ctr_TV(eagitest::case_& test) {
    tvec_vector_ctr_TNV<T, 1, V>(test);
    tvec_vector_ctr_TNV<T, 2, V>(test);
    tvec_vector_ctr_TNV<T, 3, V>(test);
    tvec_vector_ctr_TNV<T, 4, V>(test);
    tvec_vector_ctr_TNV<T, 5, V>(test);
    tvec_vector_ctr_TNV<T, 6, V>(test);
    tvec_vector_ctr_TNV<T, 7, V>(test);
    tvec_vector_ctr_TNV<T, 8, V>(test);
    tvec_vector_ctr_TNV<T, 11, V>(test);
    tvec_vector_ctr_TNV<T, 13, V>(test);
    tvec_vector_ctr_TNV<T, 19, V>(test);
    tvec_vector_ctr_TNV<T, 23, V>(test);
}
//------------------------------------------------------------------------------
template <typename T>
void tvec_vector_ctr_T(eagitest::case_& test) {
    tvec_vector_ctr_TV<T, true>(test);
    tvec_vector_ctr_TV<T, false>(test);
}
//------------------------------------------------------------------------------
void tvec_vector_ctr(auto& s) {
    eagitest::case_ test{s, 36, "tvec vector constructor"};
    tvec_vector_ctr_T<int>(test);
    tvec_vector_ctr_T<float>(test);
    tvec_vector_ctr_T<double>(test);
}
//------------------------------------------------------------------------------
// tvec array constructor
//------------------------------------------------------------------------------
template <typename T, int N, bool V>
void tvec_array_ctr_TNV(eagitest::case_& test) {
    test.parameter(N, "N");
    test.parameter(V, "V");
    auto& rg{test.random()};
    T a[N];

    for(int i = 0; i < N; ++i) {
        a[i] = rg.get_any<T>();
    }

    eagine::math::tvec<T, N, V> v(a);

    for(int i = 0; i < N; ++i) {
        test.check_equal(v[i], a[i], "equal");
    }
}
//------------------------------------------------------------------------------
template <typename T, bool V>
void tvec_array_ctr_TV(eagitest::case_& test) {
    tvec_array_ctr_TNV<T, 1, V>(test);
    tvec_array_ctr_TNV<T, 2, V>(test);
    tvec_array_ctr_TNV<T, 3, V>(test);
    tvec_array_ctr_TNV<T, 4, V>(test);
    tvec_array_ctr_TNV<T, 5, V>(test);
    tvec_array_ctr_TNV<T, 6, V>(test);
    tvec_array_ctr_TNV<T, 7, V>(test);
    tvec_array_ctr_TNV<T, 8, V>(test);
    tvec_array_ctr_TNV<T, 11, V>(test);
    tvec_array_ctr_TNV<T, 13, V>(test);
    tvec_array_ctr_TNV<T, 19, V>(test);
    tvec_array_ctr_TNV<T, 23, V>(test);
}
//------------------------------------------------------------------------------
template <typename T>
void tvec_array_ctr_T(eagitest::case_& test) {
    tvec_array_ctr_TV<T, true>(test);
    tvec_array_ctr_TV<T, false>(test);
}
//------------------------------------------------------------------------------
void tvec_array_ctr(auto& s) {
    eagitest::case_ test{s, 37, "tvec vector constructor"};
    tvec_array_ctr_T<int>(test);
    tvec_array_ctr_T<float>(test);
    tvec_array_ctr_T<double>(test);
}
//------------------------------------------------------------------------------
// tvec pack constructor
//------------------------------------------------------------------------------
template <typename T, bool V>
void tvec_pack_ctr_TV(eagitest::case_& test) {
    test.parameter(V, "V");
    auto& rg{test.random()};

    const T r1 = rg.get_any<T>();
    eagine::math::tvec<T, 1, V> v1(r1);
    test.check_equal(v1[0], r1, "v1r1");

    const T r2 = rg.get_any<T>();
    eagine::math::tvec<T, 2, V> v2(r1, r2);
    test.check_equal(v2[0], r1, "v2r1");
    test.check_equal(v2[1], r2, "v2r2");

    const T r3 = rg.get_any<T>();
    eagine::math::tvec<T, 3, V> v3(r1, r2, r3);
    test.check_equal(v3[0], r1, "v3r1");
    test.check_equal(v3[1], r2, "v3r2");
    test.check_equal(v3[2], r3, "v3r3");

    const T r4 = rg.get_any<T>();
    eagine::math::tvec<T, 4, V> v4(r1, r2, r3, r4);
    test.check_equal(v4[0], r1, "v4r1");
    test.check_equal(v4[1], r2, "v4r2");
    test.check_equal(v4[2], r3, "v4r3");
    test.check_equal(v4[3], r4, "v4r4");

    const T r5 = rg.get_any<T>();
    eagine::math::tvec<T, 5, V> v5(r1, r2, r3, r4, r5);
    test.check_equal(v5[0], r1, "v5r1");
    test.check_equal(v5[1], r2, "v5r2");
    test.check_equal(v5[2], r3, "v5r3");
    test.check_equal(v5[3], r4, "v5r4");
    test.check_equal(v5[4], r5, "v5r5");

    const T r6 = rg.get_any<T>();
    eagine::math::tvec<T, 6, V> v6(r1, r2, r3, r4, r5, r6);
    test.check_equal(v6[0], r1, "v6r1");
    test.check_equal(v6[1], r2, "v6r2");
    test.check_equal(v6[2], r3, "v6r3");
    test.check_equal(v6[3], r4, "v6r4");
    test.check_equal(v6[4], r5, "v6r5");
    test.check_equal(v6[5], r6, "v6r6");
}
//------------------------------------------------------------------------------
template <typename T>
void tvec_pack_ctr_T(eagitest::case_& test) {
    tvec_pack_ctr_TV<T, true>(test);
    tvec_pack_ctr_TV<T, false>(test);
}
//------------------------------------------------------------------------------
void tvec_pack_ctr(auto& s) {
    eagitest::case_ test{s, 38, "tvec vector constructor"};
    tvec_pack_ctr_T<int>(test);
    tvec_pack_ctr_T<float>(test);
    tvec_pack_ctr_T<double>(test);
}
//------------------------------------------------------------------------------
// tvec vector + value constructor
//------------------------------------------------------------------------------
template <typename T, bool V>
void tvec_vec_val_ctr_TV(eagitest::case_& test) {
    test.parameter(V, "V");
    auto& rg{test.random()};

    const T r1 = rg.get_any<T>();
    eagine::math::tvec<T, 1, V> v10(r1);
    test.check_equal(v10[0], r1, "v01r1");

    const T r2 = rg.get_any<T>();
    eagine::math::tvec<T, 2, V> v21(v10, r2);
    test.check_equal(v21[0], r1, "v21r1");
    test.check_equal(v21[1], r2, "v21r2");

    const T r3 = rg.get_any<T>();
    eagine::math::tvec<T, 3, V> v31(v10, r3);
    test.check_equal(v31[0], r1, "v31r1");
    test.check_equal(v31[1], r3, "v31r3");
    test.check_equal(v31[2], r3, "v31r3");

    eagine::math::tvec<T, 3, V> v32(v21, r3);
    test.check_equal(v32[0], r1, "v32r1");
    test.check_equal(v32[1], r2, "v32r2");
    test.check_equal(v32[2], r3, "v32r3");

    const T r4 = rg.get_any<T>();
    eagine::math::tvec<T, 4, V> v41(v10, r4);
    test.check_equal(v41[0], r1, "v41r1");
    test.check_equal(v41[1], r4, "v41r4");
    test.check_equal(v41[2], r4, "v41r4");
    test.check_equal(v41[3], r4, "v41r4");

    eagine::math::tvec<T, 4, V> v42(v21, r4);
    test.check_equal(v42[0], r1, "v42r1");
    test.check_equal(v42[1], r2, "v42r2");
    test.check_equal(v42[2], r4, "v42r3");
    test.check_equal(v42[3], r4, "v42r4");

    eagine::math::tvec<T, 4, V> v43(v32, r4);
    test.check_equal(v43[0], r1, "v43r1");
    test.check_equal(v43[1], r2, "v43r2");
    test.check_equal(v43[2], r3, "v43r3");
    test.check_equal(v43[3], r4, "v43r4");

    const T r5 = rg.get_any<T>();
    eagine::math::tvec<T, 5, V> v51(v10, r5);
    test.check_equal(v51[0], r1, "v51r1");
    test.check_equal(v51[1], r5, "v51r5");
    test.check_equal(v51[2], r5, "v51r5");
    test.check_equal(v51[3], r5, "v51r5");
    test.check_equal(v51[4], r5, "v51r5");

    eagine::math::tvec<T, 5, V> v52(v21, r5);
    test.check_equal(v52[0], r1, "v52r1");
    test.check_equal(v52[1], r2, "v52r2");
    test.check_equal(v52[2], r5, "v52r5");
    test.check_equal(v52[3], r5, "v52r5");
    test.check_equal(v52[4], r5, "v52r5");

    eagine::math::tvec<T, 5, V> v53(v32, r5);
    test.check_equal(v53[0], r1, "v53r1");
    test.check_equal(v53[1], r2, "v53r2");
    test.check_equal(v53[2], r3, "v53r3");
    test.check_equal(v53[3], r5, "v53r5");
    test.check_equal(v53[4], r5, "v53r5");

    eagine::math::tvec<T, 5, V> v54(v43, r5);
    test.check_equal(v54[0], r1, "v54r1");
    test.check_equal(v54[1], r2, "v54r2");
    test.check_equal(v54[2], r3, "v54r3");
    test.check_equal(v54[3], r4, "v54r4");
    test.check_equal(v54[4], r5, "v54r5");
}
//------------------------------------------------------------------------------
template <typename T>
void tvec_vec_val_ctr_T(eagitest::case_& test) {
    tvec_vec_val_ctr_TV<T, true>(test);
    tvec_vec_val_ctr_TV<T, false>(test);
}
//------------------------------------------------------------------------------
void tvec_vec_val_ctr(auto& s) {
    eagitest::case_ test{s, 39, "tvec vector + value constructor"};
    tvec_vec_val_ctr_T<int>(test);
    tvec_vec_val_ctr_T<float>(test);
    tvec_vec_val_ctr_T<double>(test);
}
//------------------------------------------------------------------------------
// tvec vector + vector constructor
//------------------------------------------------------------------------------
template <typename T, bool V>
void tvec_vec_vec_ctr_TV(eagitest::case_& test) {
    test.parameter(V, "V");
    auto& rg{test.random()};

    T ra = rg.get_any<T>();
    eagine::math::tvec<T, 1, V> va(ra);
    test.check_equal(va[0], ra, "vara");

    T rb = rg.get_any<T>();
    eagine::math::tvec<T, 1, V> vb(rb);
    test.check_equal(vb[0], rb, "vbrb");

    eagine::math::tvec<T, 2, V> vaa(va, va);
    test.check_equal(vaa[0], ra, "vaa0ra");
    test.check_equal(vaa[1], ra, "vaa1ra");

    eagine::math::tvec<T, 2, V> vab(va, vb);
    test.check_equal(vab[0], ra, "vab0ra");
    test.check_equal(vab[1], rb, "vab1rb");

    eagine::math::tvec<T, 2, V> vbb(vb, vb);
    test.check_equal(vbb[0], rb, "vbb0rb");
    test.check_equal(vbb[1], rb, "vbb1rb");

    eagine::math::tvec<T, 3, V> vaba(vab, va);
    test.check_equal(vaba[0], ra, "vaba0ra");
    test.check_equal(vaba[1], rb, "vaba1rb");
    test.check_equal(vaba[2], ra, "vaba2ra");

    eagine::math::tvec<T, 3, V> vbab(vb, vab);
    test.check_equal(vbab[0], rb, "vbab0rb");
    test.check_equal(vbab[1], ra, "vbab1ra");
    test.check_equal(vbab[2], rb, "vbab2rb");

    eagine::math::tvec<T, 4, V> vabab(vaba, vb);
    test.check_equal(vabab[0], ra, "vabab0ra");
    test.check_equal(vabab[1], rb, "vabab1rb");
    test.check_equal(vabab[2], ra, "vabab2ra");
    test.check_equal(vabab[3], rb, "vabab3rb");

    eagine::math::tvec<T, 4, V> vaabb(vaa, vbb);
    test.check_equal(vaabb[0], ra, "vaabb0ra");
    test.check_equal(vaabb[1], ra, "vaabb1ra");
    test.check_equal(vaabb[2], rb, "vaabb2rb");
    test.check_equal(vaabb[3], rb, "vaabb3rb");

    eagine::math::tvec<T, 5, V> vababa(vabab, va);
    test.check_equal(vababa[0], ra, "vababa0ra");
    test.check_equal(vababa[1], rb, "vababa1rb");
    test.check_equal(vababa[2], ra, "vababa2ra");
    test.check_equal(vababa[3], rb, "vababa3rb");
    test.check_equal(vababa[4], ra, "vababa4ra");

    eagine::math::tvec<T, 5, V> vababb(vaba, vbb);
    test.check_equal(vababb[0], ra, "vababb0ra");
    test.check_equal(vababb[1], rb, "vababb1rb");
    test.check_equal(vababb[2], ra, "vababb2ra");
    test.check_equal(vababb[3], rb, "vababb3rb");
    test.check_equal(vababb[4], rb, "vababb4rb");

    eagine::math::tvec<T, 5, V> vaabab(vaa, vbab);
    test.check_equal(vaabab[0], ra, "vaabab0ra");
    test.check_equal(vaabab[1], ra, "vaabab1ra");
    test.check_equal(vaabab[2], rb, "vaabab2rb");
    test.check_equal(vaabab[3], ra, "vaabab3ra");
    test.check_equal(vaabab[4], rb, "vaabab4rb");

    eagine::math::tvec<T, 5, V> vbabab(vb, vabab);
    test.check_equal(vbabab[0], rb, "vbabab0rb");
    test.check_equal(vbabab[1], ra, "vbabab1ra");
    test.check_equal(vbabab[2], rb, "vbabab2rb");
    test.check_equal(vbabab[3], ra, "vbabab3ra");
    test.check_equal(vbabab[4], rb, "vbabab4rb");
}
//------------------------------------------------------------------------------
template <typename T>
void tvec_vec_vec_ctr_T(eagitest::case_& test) {
    tvec_vec_vec_ctr_TV<T, true>(test);
    tvec_vec_vec_ctr_TV<T, false>(test);
}
//------------------------------------------------------------------------------
void tvec_vec_vec_ctr(auto& s) {
    eagitest::case_ test{s, 40, "tvec vector + vector constructor"};
    tvec_vec_vec_ctr_T<int>(test);
    tvec_vec_vec_ctr_T<float>(test);
    tvec_vec_vec_ctr_T<double>(test);
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "vector", 39};
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
    test.once(vector_from2_int);
    test.once(vector_from2_float);
    test.once(vector_from2_double);
    test.once(vector_from3_int);
    test.once(vector_from3_float);
    test.once(vector_from3_double);
    test.once(vector_from4_int);
    test.once(vector_from4_float);
    test.once(vector_from4_double);
    test.once(vector_dimension);
    test.once(vector_magnitude);
    test.once(vector_dot);
    test.once(tvec_default_ctr);
    test.once(tvec_fill_ctr);
    test.once(tvec_vector_ctr);
    test.once(tvec_array_ctr);
    test.once(tvec_pack_ctr);
    test.once(tvec_vec_val_ctr);
    test.once(tvec_vec_vec_ctr);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
