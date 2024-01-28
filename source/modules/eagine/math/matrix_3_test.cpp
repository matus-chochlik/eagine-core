
/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.types;
import eagine.core.math;
//------------------------------------------------------------------------------
template <typename T, int M, int N, int K, bool RM1, bool RM2, bool V>
void math_matrix_mult_TCRRMV(eagitest::case_& test) {
    T d1[M * N];

    for(int i = 0; i < (M * N); ++i) {
        d1[i] = test.random().get_between<T>(0, 10000);
    }

    T d2[N * K];

    for(int i = 0; i < (N * K); ++i) {
        d2[i] = test.random().get_between<T>(0, 10000);
    }

    auto m1 = eagine::math::matrix<T, N, M, RM1, V>::from(d1, M * N);
    auto m2 = eagine::math::matrix<T, K, N, RM2, V>::from(d2, N * K);

    eagine::math::matrix<T, K, M, RM1, V> m = multiply(m1, m2);

    for(int i = 0; i < M; ++i)
        for(int j = 0; j < K; ++j) {
            T e = T(0);

            for(int k = 0; k < N; ++k) {
                e += row(m1, i)[k] * column(m2, j)[k];
            }

            test.check_equal(get_cm(m, j, i), e, "A");
            test.check_equal(get_rm(m, i, j), e, "B");
        }
}
//------------------------------------------------------------------------------
template <typename T, bool RM1, bool RM2, bool V>
void math_matrix_mult_TRM1RM2V(eagitest::case_& test) {
    math_matrix_mult_TCRRMV<T, 1, 1, 1, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 1, 1, 2, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 1, 1, 3, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 1, 1, 4, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 1, 1, 5, RM1, RM2, V>(test);

    math_matrix_mult_TCRRMV<T, 1, 2, 1, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 1, 2, 2, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 1, 2, 3, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 1, 2, 4, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 1, 2, 5, RM1, RM2, V>(test);

    math_matrix_mult_TCRRMV<T, 1, 3, 1, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 1, 3, 2, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 1, 3, 3, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 1, 3, 4, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 1, 3, 5, RM1, RM2, V>(test);

    math_matrix_mult_TCRRMV<T, 1, 4, 1, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 1, 4, 2, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 1, 4, 3, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 1, 4, 4, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 1, 4, 5, RM1, RM2, V>(test);

    math_matrix_mult_TCRRMV<T, 1, 5, 1, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 1, 5, 2, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 1, 5, 3, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 1, 5, 4, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 1, 5, 5, RM1, RM2, V>(test);

    math_matrix_mult_TCRRMV<T, 2, 1, 1, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 2, 1, 2, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 2, 1, 3, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 2, 1, 4, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 2, 1, 5, RM1, RM2, V>(test);

    math_matrix_mult_TCRRMV<T, 2, 2, 1, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 2, 2, 2, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 2, 2, 3, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 2, 2, 4, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 2, 2, 5, RM1, RM2, V>(test);

    math_matrix_mult_TCRRMV<T, 2, 3, 1, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 2, 3, 2, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 2, 3, 3, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 2, 3, 4, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 2, 3, 5, RM1, RM2, V>(test);

    math_matrix_mult_TCRRMV<T, 2, 4, 1, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 2, 4, 2, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 2, 4, 3, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 2, 4, 4, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 2, 4, 5, RM1, RM2, V>(test);

    math_matrix_mult_TCRRMV<T, 2, 5, 1, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 2, 5, 2, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 2, 5, 3, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 2, 5, 4, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 2, 5, 5, RM1, RM2, V>(test);

    math_matrix_mult_TCRRMV<T, 3, 1, 1, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 3, 1, 2, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 3, 1, 3, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 3, 1, 4, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 3, 1, 5, RM1, RM2, V>(test);

    math_matrix_mult_TCRRMV<T, 3, 2, 1, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 3, 2, 2, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 3, 2, 3, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 3, 2, 4, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 3, 2, 5, RM1, RM2, V>(test);

    math_matrix_mult_TCRRMV<T, 3, 3, 1, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 3, 3, 2, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 3, 3, 3, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 3, 3, 4, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 3, 3, 5, RM1, RM2, V>(test);

    math_matrix_mult_TCRRMV<T, 3, 4, 1, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 3, 4, 2, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 3, 4, 3, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 3, 4, 4, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 3, 4, 5, RM1, RM2, V>(test);

    math_matrix_mult_TCRRMV<T, 3, 5, 1, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 3, 5, 2, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 3, 5, 3, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 3, 5, 4, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 3, 5, 5, RM1, RM2, V>(test);

    math_matrix_mult_TCRRMV<T, 4, 1, 1, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 4, 1, 2, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 4, 1, 3, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 4, 1, 4, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 4, 1, 5, RM1, RM2, V>(test);

    math_matrix_mult_TCRRMV<T, 4, 2, 1, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 4, 2, 2, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 4, 2, 3, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 4, 2, 4, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 4, 2, 5, RM1, RM2, V>(test);

    math_matrix_mult_TCRRMV<T, 4, 3, 1, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 4, 3, 2, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 4, 3, 3, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 4, 3, 4, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 4, 3, 5, RM1, RM2, V>(test);

    math_matrix_mult_TCRRMV<T, 4, 4, 1, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 4, 4, 2, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 4, 4, 3, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 4, 4, 4, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 4, 4, 5, RM1, RM2, V>(test);

    math_matrix_mult_TCRRMV<T, 4, 5, 1, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 4, 5, 2, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 4, 5, 3, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 4, 5, 4, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 4, 5, 5, RM1, RM2, V>(test);

    math_matrix_mult_TCRRMV<T, 5, 1, 1, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 5, 1, 2, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 5, 1, 3, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 5, 1, 4, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 5, 1, 5, RM1, RM2, V>(test);

    math_matrix_mult_TCRRMV<T, 5, 2, 1, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 5, 2, 2, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 5, 2, 3, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 5, 2, 4, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 5, 2, 5, RM1, RM2, V>(test);

    math_matrix_mult_TCRRMV<T, 5, 3, 1, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 5, 3, 2, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 5, 3, 3, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 5, 3, 4, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 5, 3, 5, RM1, RM2, V>(test);

    math_matrix_mult_TCRRMV<T, 5, 4, 1, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 5, 4, 2, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 5, 4, 3, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 5, 4, 4, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 5, 4, 5, RM1, RM2, V>(test);

    math_matrix_mult_TCRRMV<T, 5, 5, 1, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 5, 5, 2, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 5, 5, 3, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 5, 5, 4, RM1, RM2, V>(test);
    math_matrix_mult_TCRRMV<T, 5, 5, 5, RM1, RM2, V>(test);
}
//------------------------------------------------------------------------------
template <typename T, bool RM, bool V>
void math_matrix_mult_TRMV(eagitest::case_& test) {
    math_matrix_mult_TRM1RM2V<T, RM, true, V>(test);
    math_matrix_mult_TRM1RM2V<T, RM, false, V>(test);
}
//------------------------------------------------------------------------------
template <typename T>
void math_matrix_mult_T(eagitest::case_& test) {
    math_matrix_mult_TRMV<T, false, false>(test);
    math_matrix_mult_TRMV<T, false, true>(test);
    math_matrix_mult_TRMV<T, true, false>(test);
    math_matrix_mult_TRMV<T, true, true>(test);
}
//------------------------------------------------------------------------------
void math_matrix_multiply(auto& s) {
    eagitest::case_ test{s, 1, "multiply"};
    math_matrix_mult_T<float>(test);
    math_matrix_mult_T<double>(test);
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "matrix", 1};
    test.once(math_matrix_multiply);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
