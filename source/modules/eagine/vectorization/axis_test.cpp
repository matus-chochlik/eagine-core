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
template <typename T, int I, int N, bool V>
void vect_axis_TINV_1(eagitest::case_& test) {
    auto& rg{test.random()};

    for(unsigned k = 0; k < test.repeats(1000); ++k) {
        T a = rg.get_any<T>();

        typename eagine::vect::data<T, N, V>::type v =
          eagine::vect::axis<T, N, I, V>::apply(a);

        for(int i = 0; i < N; ++i) {
            test.check_equal(v[i], T(i == I ? a : 0), "element ok");
        }
    }
}
//------------------------------------------------------------------------------
template <typename T, bool V>
void vect_axis_TV_1(eagitest::case_& test) {
    vect_axis_TINV_1<T, 0, 2, V>(test);
    vect_axis_TINV_1<T, 1, 2, V>(test);
    vect_axis_TINV_1<T, 2, 2, V>(test);

    vect_axis_TINV_1<T, 0, 3, V>(test);
    vect_axis_TINV_1<T, 1, 3, V>(test);
    vect_axis_TINV_1<T, 2, 3, V>(test);
    vect_axis_TINV_1<T, 3, 3, V>(test);

    vect_axis_TINV_1<T, 0, 4, V>(test);
    vect_axis_TINV_1<T, 1, 4, V>(test);
    vect_axis_TINV_1<T, 2, 4, V>(test);
    vect_axis_TINV_1<T, 3, 4, V>(test);
    vect_axis_TINV_1<T, 4, 4, V>(test);

    vect_axis_TINV_1<T, 0, 5, V>(test);
    vect_axis_TINV_1<T, 1, 5, V>(test);
    vect_axis_TINV_1<T, 2, 5, V>(test);
    vect_axis_TINV_1<T, 3, 5, V>(test);
    vect_axis_TINV_1<T, 4, 5, V>(test);
    vect_axis_TINV_1<T, 5, 5, V>(test);

    vect_axis_TINV_1<T, 0, 6, V>(test);
    vect_axis_TINV_1<T, 1, 6, V>(test);
    vect_axis_TINV_1<T, 2, 6, V>(test);
    vect_axis_TINV_1<T, 3, 6, V>(test);
    vect_axis_TINV_1<T, 4, 6, V>(test);
    vect_axis_TINV_1<T, 5, 6, V>(test);
    vect_axis_TINV_1<T, 6, 6, V>(test);

    vect_axis_TINV_1<T, 0, 7, V>(test);
    vect_axis_TINV_1<T, 1, 7, V>(test);
    vect_axis_TINV_1<T, 2, 7, V>(test);
    vect_axis_TINV_1<T, 3, 7, V>(test);
    vect_axis_TINV_1<T, 4, 7, V>(test);
    vect_axis_TINV_1<T, 5, 7, V>(test);
    vect_axis_TINV_1<T, 6, 7, V>(test);
    vect_axis_TINV_1<T, 7, 7, V>(test);

    vect_axis_TINV_1<T, 0, 8, V>(test);
    vect_axis_TINV_1<T, 1, 8, V>(test);
    vect_axis_TINV_1<T, 2, 8, V>(test);
    vect_axis_TINV_1<T, 3, 8, V>(test);
    vect_axis_TINV_1<T, 4, 8, V>(test);
    vect_axis_TINV_1<T, 5, 8, V>(test);
    vect_axis_TINV_1<T, 6, 8, V>(test);
    vect_axis_TINV_1<T, 7, 8, V>(test);
    vect_axis_TINV_1<T, 8, 8, V>(test);
}
//------------------------------------------------------------------------------
template <typename T>
void vect_axis_T_1(eagitest::case_& test) {
    vect_axis_TV_1<T, true>(test);
    vect_axis_TV_1<T, false>(test);
}
//------------------------------------------------------------------------------
void vect_axis_1(auto& s) {
    eagitest::case_ test{s, 1, "1"};

    vect_axis_T_1<int>(test);
    vect_axis_T_1<float>(test);
    vect_axis_T_1<double>(test);
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "vect_axis", 1};
    test.once(vect_axis_1);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
