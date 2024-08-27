
/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import std;
import eagine.core.types;
import eagine.core.math;
import eagine.core.simd;
//------------------------------------------------------------------------------
template <typename T, int N, bool V, int K, int... I>
static auto get_math_matrix_vec(
  std::integral_constant<int, K>,
  std::integer_sequence<int, I...>) -> eagine::vect::data_t<T, N, V> {
    return eagine::vect::data_t<T, N, V>{T(K + I)...};
}
//------------------------------------------------------------------------------
template <typename T, int C, int R, bool RM, bool V, int... J, int... I>
void math_matrix_init_TCRRMVJI(
  eagitest::case_& test,
  std::integer_sequence<int, J...>,
  std::integer_sequence<int, I...>) {
    using M = eagine::math::matrix<T, C, R, RM, V>;

    M m = {{get_math_matrix_vec<T, (RM ? C : R), V>(
      std::integral_constant<int, J>(),
      std::integer_sequence<int, I...>())...}};

    for(int i = 0; i < (RM ? R : C); ++i)
        for(int j = 0; j < (RM ? C : R); ++j) {
            test.check_equal(m[i][j], i + j, "equal");
        }
}
//------------------------------------------------------------------------------
template <typename T, int C, int R, bool RM, bool V>
void math_matrix_init_TCRRMV(eagitest::case_& test) {
    math_matrix_init_TCRRMVJI<T, C, R, RM, V>(
      test,
      std::make_integer_sequence<int, (RM ? R : C)>(),
      std::make_integer_sequence<int, (RM ? C : R)>());
}
//------------------------------------------------------------------------------
template <typename T, bool RM, bool V>
void math_matrix_init_TRMV(eagitest::case_& test) {
    math_matrix_init_TCRRMV<T, 1, 1, RM, V>(test);
    math_matrix_init_TCRRMV<T, 1, 2, RM, V>(test);
    math_matrix_init_TCRRMV<T, 1, 3, RM, V>(test);
    math_matrix_init_TCRRMV<T, 1, 4, RM, V>(test);
    math_matrix_init_TCRRMV<T, 1, 5, RM, V>(test);

    math_matrix_init_TCRRMV<T, 2, 1, RM, V>(test);
    math_matrix_init_TCRRMV<T, 2, 2, RM, V>(test);
    math_matrix_init_TCRRMV<T, 2, 3, RM, V>(test);
    math_matrix_init_TCRRMV<T, 2, 4, RM, V>(test);
    math_matrix_init_TCRRMV<T, 2, 5, RM, V>(test);

    math_matrix_init_TCRRMV<T, 3, 1, RM, V>(test);
    math_matrix_init_TCRRMV<T, 3, 2, RM, V>(test);
    math_matrix_init_TCRRMV<T, 3, 3, RM, V>(test);
    math_matrix_init_TCRRMV<T, 3, 4, RM, V>(test);
    math_matrix_init_TCRRMV<T, 3, 5, RM, V>(test);

    math_matrix_init_TCRRMV<T, 4, 1, RM, V>(test);
    math_matrix_init_TCRRMV<T, 4, 2, RM, V>(test);
    math_matrix_init_TCRRMV<T, 4, 3, RM, V>(test);
    math_matrix_init_TCRRMV<T, 4, 4, RM, V>(test);
    math_matrix_init_TCRRMV<T, 4, 5, RM, V>(test);

    math_matrix_init_TCRRMV<T, 5, 1, RM, V>(test);
    math_matrix_init_TCRRMV<T, 5, 2, RM, V>(test);
    math_matrix_init_TCRRMV<T, 5, 3, RM, V>(test);
    math_matrix_init_TCRRMV<T, 5, 4, RM, V>(test);
    math_matrix_init_TCRRMV<T, 5, 5, RM, V>(test);
}
//------------------------------------------------------------------------------
template <typename T>
void math_matrix_init_T(eagitest::case_& test) {
    math_matrix_init_TRMV<T, true, true>(test);
    math_matrix_init_TRMV<T, true, false>(test);
    math_matrix_init_TRMV<T, false, true>(test);
    math_matrix_init_TRMV<T, false, false>(test);
}
//------------------------------------------------------------------------------
void math_matrix_init_vec(auto& s) {
    eagitest::case_ test{s, 1, "init vec"};
    math_matrix_init_T<float>(test);
    math_matrix_init_T<double>(test);
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "matrix", 1};
    test.once(math_matrix_init_vec);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
