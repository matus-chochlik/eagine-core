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
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "anything", 15};
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
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
