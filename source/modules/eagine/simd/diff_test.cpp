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
void vect_diff_TNV_1(eagitest::case_& test) {
    test.parameter(N, "N");
    test.parameter(V, "V");
    auto& rg{test.random()};

    for(unsigned k = 0; k < test.repeats(1000); ++k) {
        T a[N];

        typename eagine::simd::data<T, N, V>::type u = {}, v = {};

        for(int i = 0; i < N; ++i) {
            a[i] = rg.get_between<T>(-10000, +10000);
            u[i] = a[i];
            v[i] = a[i];
        }

        using _diff = eagine::simd::diff<T, N, V>;
        using _esum = eagine::simd::esum<T, N, V>;

        test.check_equal(_esum::apply(_diff::apply(u, u)), T(0), "u=u");
        test.check_equal(_esum::apply(_diff::apply(v, u)), T(0), "v=u");
        test.check_equal(_esum::apply(_diff::apply(u, v)), T(0), "u=v");
        test.check_equal(_esum::apply(_diff::apply(v, v)), T(0), "v=v");
    }
}
//------------------------------------------------------------------------------
template <typename T, bool V>
void vect_diff_TV_1(eagitest::case_& test) {
    vect_diff_TNV_1<T, 2, V>(test);
    vect_diff_TNV_1<T, 3, V>(test);
    vect_diff_TNV_1<T, 4, V>(test);
    vect_diff_TNV_1<T, 5, V>(test);
    vect_diff_TNV_1<T, 7, V>(test);
    vect_diff_TNV_1<T, 8, V>(test);
    vect_diff_TNV_1<T, 11, V>(test);
    vect_diff_TNV_1<T, 15, V>(test);
    vect_diff_TNV_1<T, 19, V>(test);
}
//------------------------------------------------------------------------------
template <typename T>
void vect_diff_T_1(eagitest::case_& test) {
    vect_diff_TV_1<T, true>(test);
    vect_diff_TV_1<T, false>(test);
}
//------------------------------------------------------------------------------
void vect_diff_int_1(auto& s) {
    eagitest::case_ test{s, 1, "int 1"};
    vect_diff_T_1<int>(test);
}
//------------------------------------------------------------------------------
void vect_diff_float_1(auto& s) {
    eagitest::case_ test{s, 2, "float 1"};
    vect_diff_T_1<float>(test);
}
//------------------------------------------------------------------------------
void vect_diff_double_1(auto& s) {
    eagitest::case_ test{s, 3, "double 1"};
    vect_diff_T_1<double>(test);
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "vect_diff", 3};
    test.once(vect_diff_int_1);
    test.once(vect_diff_float_1);
    test.once(vect_diff_double_1);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
