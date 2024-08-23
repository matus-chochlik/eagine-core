
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
import eagine.core.units;
//------------------------------------------------------------------------------
template <typename MC>
void math_matrix_ctr_MC(eagitest::case_& test, MC mc) {
    using namespace eagine::math;

    static_assert(is_matrix_constructor<MC>::value);

    auto m1 = construct_matrix<true>(mc);
    auto m2 = construct_matrix<false>(mc);

    test.ensure(m1.rows() == m2.rows(), "rows 1,2");
    test.ensure(m1.columns() == m2.columns(), "columns 1,2");

    test.check_equal(m1.is_row_major(), not m2.is_row_major(), "row major 1");
    test.check_equal(m2.is_row_major(), not m1.is_row_major(), "row major 2");

    for(int i = 0; i < m1.rows(); ++i)
        for(int j = 0; j < m1.columns(); ++j) {
            test.check_equal(m1.get_rm(i, j), m2.get_cm(j, i), "RMCM 1");
            test.check_equal(m2.get_rm(i, j), m1.get_cm(j, i), "RMCM 2");
        }

    auto m3 = construct_matrix<true>(multiply(m1, m2));
    auto m4 = construct_matrix<true>(multiply(mc, mc));

    test.ensure(m3.rows() == m4.rows(), "rows 3,4");
    test.ensure(m3.columns() == m4.columns(), "columns 3,4");

    test.check(m3.is_row_major(), "row major 3");
    test.check(m4.is_row_major(), "row major 4");

    for(int i = 0; i < m3.rows(); ++i) {
        for(int j = 0; j < m3.columns(); ++j) {
            test.check_close(m3.get_rm(i, j), m4.get_cm(j, i), "get 1");
            test.check_close(m4.get_rm(i, j), m3.get_cm(j, i), "get 2");
        }
    }
}
//------------------------------------------------------------------------------
template <typename T, bool RM, bool V>
void math_matrix_ctrs_TRMV(eagitest::case_& test) {
    using namespace eagine::math;
    using eagine::radians_;

    const auto r = [&]() {
        return test.random().get_between<T>(1, 10000);
    };

    math_matrix_ctr_MC(test, identity<matrix<T, 4, 4, RM, V>>());
    math_matrix_ctr_MC(
      test, translation<matrix<T, 4, 4, RM, V>>(r(), r(), r()));
    math_matrix_ctr_MC(test, scale<matrix<T, 4, 4, RM, V>>(r(), r(), r()));
    math_matrix_ctr_MC(test, uniform_scale<matrix<T, 4, 4, RM, V>>(r()));
    math_matrix_ctr_MC(test, rotation_x<matrix<T, 4, 4, RM, V>>(radians_(r())));
    math_matrix_ctr_MC(test, rotation_y<matrix<T, 4, 4, RM, V>>(radians_(r())));
    math_matrix_ctr_MC(test, rotation_z<matrix<T, 4, 4, RM, V>>(radians_(r())));
    math_matrix_ctr_MC(test, reflection_x<matrix<T, 4, 4, RM, V>>(true));
    math_matrix_ctr_MC(test, reflection_y<matrix<T, 4, 4, RM, V>>(true));
    math_matrix_ctr_MC(test, reflection_z<matrix<T, 4, 4, RM, V>>(true));

    math_matrix_ctr_MC(
      test,
      orbiting_y_up<matrix<T, 4, 4, RM, V>>(
        vector<T, 3, V>{{r(), r(), r()}}, r(), radians_(r()), radians_(r())));
    math_matrix_ctr_MC(
      test,
      looking_at_y_up<matrix<T, 4, 4, RM, V>>(
        vector<T, 3, V>{{r(), r(), r()}}, vector<T, 3, V>{{r(), r(), r()}}));

    math_matrix_ctr_MC(
      test, ortho<matrix<T, 4, 4, RM, V>>(-r(), +r(), -r(), +r(), 1, 10));
    math_matrix_ctr_MC(
      test, perspective<matrix<T, 4, 4, RM, V>>(-r(), +r(), -r(), +r(), 1, 10));
}
//------------------------------------------------------------------------------
template <typename T>
void math_matrix_ctrs_T(eagitest::case_& test) {
    math_matrix_ctrs_TRMV<T, true, true>(test);
    math_matrix_ctrs_TRMV<T, true, false>(test);
    math_matrix_ctrs_TRMV<T, false, true>(test);
    math_matrix_ctrs_TRMV<T, false, false>(test);
}
//------------------------------------------------------------------------------
void math_matrix_multiply(auto& s) {
    eagitest::case_ test{s, 1, "multiply"};
    math_matrix_ctrs_T<float>(test);
    math_matrix_ctrs_T<double>(test);
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
