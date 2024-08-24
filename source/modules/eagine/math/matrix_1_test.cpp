
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
// boilerplate
//------------------------------------------------------------------------------
template <typename T, int C, int R, bool RM, bool V, class TestMaker>
void math_matrix_TCRRMVm(eagitest::case_& test, const TestMaker& m) {
    auto tester{m.template make<T, C, R, RM, V>()};
    tester(test);
}
//------------------------------------------------------------------------------
template <typename T, bool RM, bool V, class TestMaker>
void math_matrix_TRMV(eagitest::case_& test, const TestMaker& m) {
    math_matrix_TCRRMVm<T, 1, 1, RM, V>(test, m);
    math_matrix_TCRRMVm<T, 1, 2, RM, V>(test, m);
    math_matrix_TCRRMVm<T, 1, 3, RM, V>(test, m);
    math_matrix_TCRRMVm<T, 1, 4, RM, V>(test, m);
    math_matrix_TCRRMVm<T, 1, 5, RM, V>(test, m);

    math_matrix_TCRRMVm<T, 2, 1, RM, V>(test, m);
    math_matrix_TCRRMVm<T, 2, 2, RM, V>(test, m);
    math_matrix_TCRRMVm<T, 2, 3, RM, V>(test, m);
    math_matrix_TCRRMVm<T, 2, 4, RM, V>(test, m);
    math_matrix_TCRRMVm<T, 2, 5, RM, V>(test, m);

    math_matrix_TCRRMVm<T, 3, 1, RM, V>(test, m);
    math_matrix_TCRRMVm<T, 3, 2, RM, V>(test, m);
    math_matrix_TCRRMVm<T, 3, 3, RM, V>(test, m);
    math_matrix_TCRRMVm<T, 3, 4, RM, V>(test, m);
    math_matrix_TCRRMVm<T, 3, 5, RM, V>(test, m);

    math_matrix_TCRRMVm<T, 4, 1, RM, V>(test, m);
    math_matrix_TCRRMVm<T, 4, 2, RM, V>(test, m);
    math_matrix_TCRRMVm<T, 4, 3, RM, V>(test, m);
    math_matrix_TCRRMVm<T, 4, 4, RM, V>(test, m);
    math_matrix_TCRRMVm<T, 4, 5, RM, V>(test, m);

    math_matrix_TCRRMVm<T, 5, 1, RM, V>(test, m);
    math_matrix_TCRRMVm<T, 5, 2, RM, V>(test, m);
    math_matrix_TCRRMVm<T, 5, 3, RM, V>(test, m);
    math_matrix_TCRRMVm<T, 5, 4, RM, V>(test, m);
    math_matrix_TCRRMVm<T, 5, 5, RM, V>(test, m);
}
//------------------------------------------------------------------------------
template <typename T, class TestMaker>
void math_matrix_T(eagitest::case_& test, const TestMaker& m) {
    math_matrix_TRMV<T, true, true>(test, m);
    math_matrix_TRMV<T, true, false>(test, m);
    math_matrix_TRMV<T, false, true>(test, m);
    math_matrix_TRMV<T, false, false>(test, m);
}
//------------------------------------------------------------------------------
template <class TestMaker>
void math_matrix_m(eagitest::case_& test, const TestMaker& m) {
    math_matrix_T<float>(test, m);
    math_matrix_T<double>(test, m);
}
//------------------------------------------------------------------------------
// default construct
//------------------------------------------------------------------------------
struct matrix_default_ctr_test_maker {
    template <typename T, int C, int R, bool RM, bool V>
    struct _test {
        void operator()(eagitest::case_& test) const {
            eagine::math::matrix<T, C, R, RM, V> m;
            test.check_equal(m.rows(), R, "rows");
            test.check_equal(m.columns(), C, "columns");
            test.check_equal(m.is_row_major(), RM, "is row major");
        }
    };

    template <typename T, int C, int R, bool RM, bool V>
    static auto make() -> _test<T, C, R, RM, V> {
        return {};
    }
};
//------------------------------------------------------------------------------
void math_matrix_default_construct(auto& s) {
    eagitest::case_ test{s, 1, "default construct"};
    matrix_default_ctr_test_maker m;
    math_matrix_m(test, m);
}
//------------------------------------------------------------------------------
// from 1
//------------------------------------------------------------------------------
struct matrix_from1_test_maker {
    template <typename T, int C, int R, bool RM, bool V>
    struct _test {
        void operator()(eagitest::case_& test) const {
            T d[C * R];

            for(int i = 0; i < (C * R); ++i) {
                d[i] = test.random().get_between<T>(-5000, 5000);
            }

            auto m = eagine::math::matrix<T, C, R, RM, V>::from(d, C * R);

            for(int i = 0; i < (RM ? R : C); ++i)
                for(int j = 0; j < (RM ? C : R); ++j) {
                    test.check_equal(m[i][j], d[(RM ? C : R) * i + j], "equal");
                }
        }
    };

    template <typename T, int C, int R, bool RM, bool V>
    static auto make() -> _test<T, C, R, RM, V> {
        return {};
    }
};
//------------------------------------------------------------------------------
void math_matrix_from_1(auto& s) {
    eagitest::case_ test{s, 2, "from 1"};
    matrix_from1_test_maker m;
    math_matrix_m(test, m);
}
//------------------------------------------------------------------------------
// from 2
//------------------------------------------------------------------------------
struct matrix_from2_test_maker {
    template <typename T, int C, int R, bool RM, bool V>
    struct _test {
        template <int I>
        using _uint = std::integral_constant<int, I>;

        template <int M, int N>
        static void _do_test(
          eagitest::case_& test,
          _uint<M>,
          _uint<N>,
          const eagine::math::matrix<T, C, R, RM, V>& m) {
            auto n = eagine::math::matrix<T, M, N, RM, V>::from(m);
            for(int i = 0; i < (RM ? N : M); ++i)
                for(int j = 0; j < (RM ? M : N); ++j) {
                    test.check_equal(n[i][j], m[i][j], "equal");
                }
        }

        template <int N>
        static void _call_test(
          eagitest::case_&,
          _uint<0>,
          _uint<N>,
          const eagine::math::matrix<T, C, R, RM, V>&) {}

        template <int M>
        static void _call_test(
          eagitest::case_& test,
          _uint<M>,
          _uint<0>,
          const eagine::math::matrix<T, C, R, RM, V>& m) {
            _call_test(test, _uint<M - 1>(), _uint<R>(), m);
        }

        template <int M, int N>
        static void _call_test(
          eagitest::case_& test,
          _uint<M>,
          _uint<N>,
          const eagine::math::matrix<T, C, R, RM, V>& m) {
            _do_test(test, _uint<M>(), _uint<N>(), m);
            _call_test(test, _uint<M>(), _uint<N - 1>(), m);
        }

        void operator()(eagitest::case_& test) const {
            T d[C * R];

            for(int i = 0; i < (C * R); ++i) {
                d[i] = test.random().get_between<T>(-5000, 5000);
            }

            auto m{eagine::math::matrix<T, C, R, RM, V>::from(d, C * R)};
            _call_test(test, _uint<C>(), _uint<R>(), m);
        }
    };

    template <typename T, int C, int R, bool RM, bool V>
    static auto make() -> _test<T, C, R, RM, V> {
        return {};
    }
};
//------------------------------------------------------------------------------
void math_matrix_from_2(auto& s) {
    eagitest::case_ test{s, 3, "from 2"};
    matrix_from2_test_maker m;
    math_matrix_m(test, m);
}
//------------------------------------------------------------------------------
// get/set
//------------------------------------------------------------------------------
struct matrix_get_set_test_maker {
    template <typename T, int C, int R, bool RM, bool V>
    struct _test {
        void operator()(eagitest::case_& test) const {
            T d[C * R];

            for(int i = 0; i < (C * R); ++i) {
                d[i] = test.random().get_between<T>(-5000, 5000);
            }

            auto m = eagine::math::matrix<T, C, R, RM, V>::from(d, C * R);

            for(int i = 0; i < (RM ? R : C); ++i)
                for(int j = 0; j < (RM ? C : R); ++j) {
                    test.check_equal(m[i][j], d[(RM ? C : R) * i + j], "A");
                }

            for(int i = 0; i < C; ++i)
                for(int j = 0; j < R; ++j) {
                    m.set_cm(i, j, m.get_cm(i, j) + 1);
                }

            for(int i = 0; i < R; ++i)
                for(int j = 0; j < C; ++j) {
                    test.check_equal(m.get_rm(i, j), m.get_cm(j, i), "B");
                }

            for(int i = 0; i < (RM ? R : C); ++i)
                for(int j = 0; j < (RM ? C : R); ++j) {
                    test.check_equal(m[i][j], d[(RM ? C : R) * i + j] + 1, "C");
                }
        }
    };

    template <typename T, int C, int R, bool RM, bool V>
    static _test<T, C, R, RM, V> make() {
        return {};
    }
};
//------------------------------------------------------------------------------
void math_matrix_get_set(auto& s) {
    eagitest::case_ test{s, 4, "get & set"};
    matrix_get_set_test_maker m;
    math_matrix_m(test, m);
}
//------------------------------------------------------------------------------
// transpose
//------------------------------------------------------------------------------
struct matrix_transpose_test_maker {
    template <typename T, int M, int N, bool RM, bool V>
    struct _test {
        void operator()(eagitest::case_& test) const {
            T d[M * N];

            for(int i = 0; i < (M * N); ++i) {
                d[i] = test.random().get_between<T>(-5000, 5000);
            }

            auto m = eagine::math::matrix<T, M, N, RM, V>::from(d, M * N);
            eagine::math::matrix<T, N, M, RM, V> n = m.transposed();

            for(int i = 0; i < M; ++i)
                for(int j = 0; j < N; ++j) {
                    test.check_equal(m.get_cm(i, j), n.get_cm(j, i), "CM");
                    test.check_equal(m.get_rm(j, i), n.get_rm(i, j), "RM");
                }
        }
    };

    template <typename T, int C, int R, bool RM, bool V>
    static _test<T, C, R, RM, V> make() {
        return {};
    }
};
//------------------------------------------------------------------------------
void math_matrix_transpose(auto& s) {
    eagitest::case_ test{s, 5, "transpose"};
    matrix_transpose_test_maker m;
    math_matrix_m(test, m);
}
//------------------------------------------------------------------------------
// reorder
//------------------------------------------------------------------------------
struct matrix_reorder_test_maker {
    template <typename T, int M, int N, bool RM, bool V>
    struct _test {
        void operator()(eagitest::case_& test) const {
            T d[M * N];

            for(int i = 0; i < (M * N); ++i) {
                d[i] = test.random().get_between<T>(-5000, 5000);
            }

            auto m = eagine::math::matrix<T, M, N, RM, V>::from(d, M * N);
            eagine::math::matrix<T, M, N, not RM, V> n = m.reordered();

            test.check_equal(m.is_row_major(), not n.is_row_major(), "RMRMmn");
            test.check_equal(n.is_row_major(), not m.is_row_major(), "RMRMnm");

            for(int i = 0; i < M; ++i)
                for(int j = 0; j < N; ++j) {
                    test.check_equal(m.get_cm(i, j), n.get_cm(i, j), "CM");
                    test.check_equal(m.get_rm(j, i), n.get_rm(j, i), "RM");
                }
        }
    };

    template <typename T, int C, int R, bool RM, bool V>
    static _test<T, C, R, RM, V> make() {
        return {};
    }
};
//------------------------------------------------------------------------------
void math_matrix_reorder(auto& s) {
    eagitest::case_ test{s, 6, "reorder"};
    matrix_reorder_test_maker m;
    math_matrix_m(test, m);
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "matrix", 6};
    test.once(math_matrix_default_construct);
    test.once(math_matrix_from_1);
    test.once(math_matrix_from_2);
    test.once(math_matrix_get_set);
    test.once(math_matrix_transpose);
    test.once(math_matrix_reorder);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
