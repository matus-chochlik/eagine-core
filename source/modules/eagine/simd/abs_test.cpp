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
void vect_abs_TNV_1(eagitest::case_& test) {
    test.parameter(N, "N");
    test.parameter(V, "V");
    auto& rg{test.random()};

    for(unsigned k = 0; k < test.repeats(1000); ++k) {
        T a[N];

        typename eagine::simd::data<T, N, V>::type u = {};

        for(int i = 0; i < N; ++i) {
            a[i] = rg.get_between<T>(-1000, 1000);
            u[i] = a[i];
        }

        typename eagine::simd::data<T, N, V>::type v =
          eagine::simd::abs<T, N, V>::apply(u);

        for(int i = 0; i < N; ++i) {
            using std::abs;
            test.check(not(v[i] < T(0)), "not less than");
            test.check_equal(v[i], abs(a[i]), "compare 1");
            test.check_equal(v[i], abs(u[i]), "compare 2");
        }
    }
}
//------------------------------------------------------------------------------
template <typename T, bool V>
void vect_abs_TV_1(eagitest::case_& test) {
    vect_abs_TNV_1<T, 2, V>(test);
    vect_abs_TNV_1<T, 3, V>(test);
    vect_abs_TNV_1<T, 4, V>(test);
    vect_abs_TNV_1<T, 5, V>(test);
    vect_abs_TNV_1<T, 7, V>(test);
    vect_abs_TNV_1<T, 8, V>(test);
    vect_abs_TNV_1<T, 11, V>(test);
    vect_abs_TNV_1<T, 15, V>(test);
    vect_abs_TNV_1<T, 19, V>(test);
}
//------------------------------------------------------------------------------
template <typename T>
void vect_abs_T_1(eagitest::case_& test) {
    vect_abs_TV_1<T, true>(test);
    vect_abs_TV_1<T, false>(test);
}
//------------------------------------------------------------------------------
void vect_abs_int_1(auto& s) {
    eagitest::case_ test{s, 1, "int 1"};
    vect_abs_T_1<int>(test);
}
//------------------------------------------------------------------------------
void vect_abs_float_1(auto& s) {
    eagitest::case_ test{s, 2, "float 1"};
    vect_abs_T_1<float>(test);
}
//------------------------------------------------------------------------------
void vect_abs_double_1(auto& s) {
    eagitest::case_ test{s, 3, "double 1"};
    vect_abs_T_1<double>(test);
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "vect_abs", 3};
    test.once(vect_abs_int_1);
    test.once(vect_abs_float_1);
    test.once(vect_abs_double_1);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
