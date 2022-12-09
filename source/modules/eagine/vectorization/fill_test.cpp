/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.vectorization;
//------------------------------------------------------------------------------
template <typename T, int N, bool V>
void vect_fill_TNV_1(eagitest::case_& test) {
    test.parameter(N, "N");
    test.parameter(V, "V");
    auto& rg{test.random()};

    for(unsigned k = 0; k < test.repeats(1000); ++k) {
        T a = rg.get_any<T>();

        typename eagine::vect::data<T, N, V>::type v =
          eagine::vect::fill<T, N, V>::apply(a);

        for(int i = 0; i < N; ++i) {
            test.check_equal(v[i], a, "equal");
        }
    }
}
//------------------------------------------------------------------------------
template <typename T, bool V>
void vect_fill_TV_1(eagitest::case_& test) {
    vect_fill_TNV_1<T, 2, V>(test);
    vect_fill_TNV_1<T, 3, V>(test);
    vect_fill_TNV_1<T, 4, V>(test);
    vect_fill_TNV_1<T, 5, V>(test);
    vect_fill_TNV_1<T, 6, V>(test);
    vect_fill_TNV_1<T, 7, V>(test);
    vect_fill_TNV_1<T, 8, V>(test);
}
//------------------------------------------------------------------------------
template <typename T>
void vect_fill_T_1(eagitest::case_& test) {
    vect_fill_TV_1<T, true>(test);
    vect_fill_TV_1<T, false>(test);
}
//------------------------------------------------------------------------------
void vect_fill_int_1(auto& s) {
    eagitest::case_ test{s, 1, "int 1"};
    vect_fill_T_1<int>(test);
}
//------------------------------------------------------------------------------
void vect_fill_float_1(auto& s) {
    eagitest::case_ test{s, 2, "float 1"};
    vect_fill_T_1<float>(test);
}
//------------------------------------------------------------------------------
void vect_fill_double_1(auto& s) {
    eagitest::case_ test{s, 3, "double 1"};
    vect_fill_T_1<double>(test);
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "vect_fill", 3};
    test.once(vect_fill_int_1);
    test.once(vect_fill_float_1);
    test.once(vect_fill_double_1);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
