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
void vect_compare_TNV_1(eagitest::case_& test) {
    test.parameter(N, "N");
    test.parameter(V, "V");
    auto& rg{test.random()};

    for(unsigned k = 0; k < test.repeats(1000); ++k) {
        bool gen_zero = rg.get_int(0, 9) == 0;
        T a = gen_zero ? 0 : rg.get_between<T>(1, 1000);

        typename eagine::vect::data<T, N, V>::type v =
          eagine::vect::fill<T, N, V>::apply(a);

        bool is_zero = eagine::vect::is_zero<T, N, V>::apply(v);

        test.check_equal(gen_zero, is_zero, "is zero");
    }
}
//------------------------------------------------------------------------------
template <typename T, bool V>
void vect_compare_TV_1(eagitest::case_& test) {
    vect_compare_TNV_1<T, 2, V>(test);
    vect_compare_TNV_1<T, 3, V>(test);
    vect_compare_TNV_1<T, 4, V>(test);
    vect_compare_TNV_1<T, 5, V>(test);
    vect_compare_TNV_1<T, 6, V>(test);
    vect_compare_TNV_1<T, 7, V>(test);
    vect_compare_TNV_1<T, 8, V>(test);
}
//------------------------------------------------------------------------------
template <typename T>
void vect_compare_T_1(eagitest::case_& test) {
    vect_compare_TV_1<T, true>(test);
    vect_compare_TV_1<T, false>(test);
}
//------------------------------------------------------------------------------
void vect_compare_int_1(auto& s) {
    eagitest::case_ test{s, 1, "int 1"};
    vect_compare_T_1<int>(test);
}
//------------------------------------------------------------------------------
void vect_compare_float_1(auto& s) {
    eagitest::case_ test{s, 2, "float 1"};
    vect_compare_T_1<float>(test);
}
//------------------------------------------------------------------------------
void vect_compare_double_1(auto& s) {
    eagitest::case_ test{s, 3, "double 1"};
    vect_compare_T_1<double>(test);
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "vect_compare", 3};
    test.once(vect_compare_int_1);
    test.once(vect_compare_float_1);
    test.once(vect_compare_double_1);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
