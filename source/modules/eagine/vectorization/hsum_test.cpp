/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.vectorization;
//------------------------------------------------------------------------------
template <typename T, int N, bool V>
void vect_hsum_TNV_1(eagitest::case_& test) {
    auto& rg{test.random()};

    for(unsigned k = 0; k < test.repeats(1000); ++k) {
        T s = T(0);

        typename eagine::vect::data<T, N, V>::type v = {};

        for(int i = 0; i < N; ++i) {
            T n = rg.get_between<T>(0, 5000);
            v[i] = n;
            s += n;
        }

        typename eagine::vect::data<T, N, V>::type r =
          eagine::vect::hsum<T, N, V>::apply(v);

        for(int i = 0; i < N; ++i) {
            test.check_close(s, r[i], "compare");
        }
    }
}
//------------------------------------------------------------------------------
template <typename T, bool V>
void vect_hsum_TV_1(eagitest::case_& test) {
    vect_hsum_TNV_1<T, 2, V>(test);
    vect_hsum_TNV_1<T, 3, V>(test);
    vect_hsum_TNV_1<T, 4, V>(test);
    vect_hsum_TNV_1<T, 5, V>(test);
    vect_hsum_TNV_1<T, 7, V>(test);
    vect_hsum_TNV_1<T, 8, V>(test);
    vect_hsum_TNV_1<T, 11, V>(test);
    vect_hsum_TNV_1<T, 17, V>(test);
    vect_hsum_TNV_1<T, 19, V>(test);
    vect_hsum_TNV_1<T, 23, V>(test);
}
//------------------------------------------------------------------------------
template <typename T>
void vect_hsum_T_1(eagitest::case_& test) {
    vect_hsum_TV_1<T, true>(test);
    vect_hsum_TV_1<T, false>(test);
}
//------------------------------------------------------------------------------
void vect_hsum_int_1(auto& s) {
    eagitest::case_ test{s, 1, "int 1"};
    vect_hsum_T_1<int>(test);
}
//------------------------------------------------------------------------------
void vect_hsum_float_1(auto& s) {
    eagitest::case_ test{s, 2, "float 1"};
    vect_hsum_T_1<float>(test);
}
//------------------------------------------------------------------------------
void vect_hsum_double_1(auto& s) {
    eagitest::case_ test{s, 3, "double 1"};
    vect_hsum_T_1<double>(test);
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "vect_hsum", 3};
    test.once(vect_hsum_int_1);
    test.once(vect_hsum_float_1);
    test.once(vect_hsum_double_1);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
