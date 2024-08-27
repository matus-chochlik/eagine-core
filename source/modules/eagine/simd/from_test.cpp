/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.simd;
//------------------------------------------------------------------------------
template <typename T, int N, bool V>
void vect_from_TNV_array(eagitest::case_& test) {
    test.parameter(N, "N");
    test.parameter(V, "V");
    auto& rg{test.random()};

    for(unsigned k = 0; k < test.repeats(1000); ++k) {
        T a[N];
        for(int i = 0; i < N; ++i) {
            a[i] = rg.get_any<T>();
        }

        typename eagine::simd::data<T, N, V>::type v =
          eagine::simd::from_array<T, N, V>::apply(a, N);

        for(int i = 0; i < N; ++i) {
            test.check_equal(v[i], a[i], "equal");
        }
    }
}
//------------------------------------------------------------------------------
template <typename T, bool V>
void vect_from_TV_array(eagitest::case_& test) {
    vect_from_TNV_array<T, 2, V>(test);
    vect_from_TNV_array<T, 3, V>(test);
    vect_from_TNV_array<T, 4, V>(test);
    vect_from_TNV_array<T, 5, V>(test);
    vect_from_TNV_array<T, 6, V>(test);
    vect_from_TNV_array<T, 7, V>(test);
    vect_from_TNV_array<T, 8, V>(test);
}
//------------------------------------------------------------------------------
template <typename T>
void vect_from_T_array(eagitest::case_& test) {
    vect_from_TV_array<T, true>(test);
    vect_from_TV_array<T, false>(test);
}
//------------------------------------------------------------------------------
void vect_from_int_array(auto& s) {
    eagitest::case_ test{s, 1, "int 1"};
    vect_from_T_array<int>(test);
}
//------------------------------------------------------------------------------
void vect_from_float_array(auto& s) {
    eagitest::case_ test{s, 2, "float 1"};
    vect_from_T_array<float>(test);
}
//------------------------------------------------------------------------------
void vect_from_double_array(auto& s) {
    eagitest::case_ test{s, 3, "double 1"};
    vect_from_T_array<double>(test);
}
//------------------------------------------------------------------------------
template <typename T, int N, int M, bool V>
void vect_from_saafv_TNMV(eagitest::case_& test) {
    test.parameter(N, "N");
    test.parameter(V, "V");
    auto& rg{test.random()};

    for(unsigned k = 0; k < test.repeats(100); ++k) {
        static_assert(M <= N, "");

        T a[M];
        for(int i = 0; i < M; ++i) {
            a[i] = rg.get_any<T>();
        }
        T f = rg.get_any<T>();

        typename eagine::simd::data<T, N, V>::type v =
          eagine::simd::from_saafv<T, N, V>::apply(a, M, f);

        for(int i = 0; i < M; ++i) {
            test.check_equal(v[i], a[i], "equal a");
        }
        for(int i = M; i < N; ++i) {
            test.check_equal(v[i], f, "equal f");
        }
    }
}
//------------------------------------------------------------------------------
template <typename T, bool V>
void vect_from_saafv_TV(eagitest::case_& test) {
    vect_from_saafv_TNMV<T, 2, 1, V>(test);
    vect_from_saafv_TNMV<T, 2, 2, V>(test);

    vect_from_saafv_TNMV<T, 3, 1, V>(test);
    vect_from_saafv_TNMV<T, 3, 2, V>(test);
    vect_from_saafv_TNMV<T, 3, 3, V>(test);

    vect_from_saafv_TNMV<T, 4, 1, V>(test);
    vect_from_saafv_TNMV<T, 4, 2, V>(test);
    vect_from_saafv_TNMV<T, 4, 3, V>(test);
    vect_from_saafv_TNMV<T, 4, 4, V>(test);

    vect_from_saafv_TNMV<T, 5, 1, V>(test);
    vect_from_saafv_TNMV<T, 5, 2, V>(test);
    vect_from_saafv_TNMV<T, 5, 3, V>(test);
    vect_from_saafv_TNMV<T, 5, 4, V>(test);
    vect_from_saafv_TNMV<T, 5, 5, V>(test);

    vect_from_saafv_TNMV<T, 6, 1, V>(test);
    vect_from_saafv_TNMV<T, 6, 2, V>(test);
    vect_from_saafv_TNMV<T, 6, 3, V>(test);
    vect_from_saafv_TNMV<T, 6, 4, V>(test);
    vect_from_saafv_TNMV<T, 6, 5, V>(test);
    vect_from_saafv_TNMV<T, 6, 6, V>(test);

    vect_from_saafv_TNMV<T, 7, 1, V>(test);
    vect_from_saafv_TNMV<T, 7, 2, V>(test);
    vect_from_saafv_TNMV<T, 7, 3, V>(test);
    vect_from_saafv_TNMV<T, 7, 4, V>(test);
    vect_from_saafv_TNMV<T, 7, 5, V>(test);
    vect_from_saafv_TNMV<T, 7, 6, V>(test);
    vect_from_saafv_TNMV<T, 7, 7, V>(test);

    vect_from_saafv_TNMV<T, 8, 1, V>(test);
    vect_from_saafv_TNMV<T, 8, 2, V>(test);
    vect_from_saafv_TNMV<T, 8, 3, V>(test);
    vect_from_saafv_TNMV<T, 8, 4, V>(test);
    vect_from_saafv_TNMV<T, 8, 5, V>(test);
    vect_from_saafv_TNMV<T, 8, 6, V>(test);
    vect_from_saafv_TNMV<T, 8, 7, V>(test);
    vect_from_saafv_TNMV<T, 8, 8, V>(test);
}
//------------------------------------------------------------------------------
template <typename T>
void vect_from_saafv_T(eagitest::case_& test) {
    vect_from_saafv_TV<T, true>(test);
    vect_from_saafv_TV<T, false>(test);
}
//------------------------------------------------------------------------------
void vect_from_int_saafv(auto& s) {
    eagitest::case_ test{s, 4, "int 2"};
    vect_from_saafv_T<int>(test);
}
//------------------------------------------------------------------------------
void vect_from_float_saafv(auto& s) {
    eagitest::case_ test{s, 5, "float 2"};
    vect_from_saafv_T<float>(test);
}
//------------------------------------------------------------------------------
void vect_from_double_saafv(auto& s) {
    eagitest::case_ test{s, 6, "double 2"};
    vect_from_saafv_T<double>(test);
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "vect_from", 6};
    test.once(vect_from_int_array);
    test.once(vect_from_float_array);
    test.once(vect_from_double_array);
    test.once(vect_from_int_saafv);
    test.once(vect_from_float_saafv);
    test.once(vect_from_double_saafv);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
