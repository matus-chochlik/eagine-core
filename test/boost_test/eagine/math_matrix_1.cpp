/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/math/matrix.hpp>
#define BOOST_TEST_MODULE EAGINE_math_matrix_1
#include "../unit_test_begin.inl"

BOOST_AUTO_TEST_SUITE(math_matrix_tests_1)

static eagine::test_random_generator rg;

template <typename T, int C, int R, bool RM, bool V, class Tester>
void test_math_matrix_TCRRMV(const Tester& t) {
    auto test = t.template make<T, C, R, RM, V>();
    test();
}

template <typename T, bool RM, bool V, class Tester>
void test_math_matrix_TRMV(const Tester& t) {
    test_math_matrix_TCRRMV<T, 1, 1, RM, V>(t);
    test_math_matrix_TCRRMV<T, 1, 2, RM, V>(t);
    test_math_matrix_TCRRMV<T, 1, 3, RM, V>(t);
    test_math_matrix_TCRRMV<T, 1, 4, RM, V>(t);
    test_math_matrix_TCRRMV<T, 1, 5, RM, V>(t);

    test_math_matrix_TCRRMV<T, 2, 1, RM, V>(t);
    test_math_matrix_TCRRMV<T, 2, 2, RM, V>(t);
    test_math_matrix_TCRRMV<T, 2, 3, RM, V>(t);
    test_math_matrix_TCRRMV<T, 2, 4, RM, V>(t);
    test_math_matrix_TCRRMV<T, 2, 5, RM, V>(t);

    test_math_matrix_TCRRMV<T, 3, 1, RM, V>(t);
    test_math_matrix_TCRRMV<T, 3, 2, RM, V>(t);
    test_math_matrix_TCRRMV<T, 3, 3, RM, V>(t);
    test_math_matrix_TCRRMV<T, 3, 4, RM, V>(t);
    test_math_matrix_TCRRMV<T, 3, 5, RM, V>(t);

    test_math_matrix_TCRRMV<T, 4, 1, RM, V>(t);
    test_math_matrix_TCRRMV<T, 4, 2, RM, V>(t);
    test_math_matrix_TCRRMV<T, 4, 3, RM, V>(t);
    test_math_matrix_TCRRMV<T, 4, 4, RM, V>(t);
    test_math_matrix_TCRRMV<T, 4, 5, RM, V>(t);

    test_math_matrix_TCRRMV<T, 5, 1, RM, V>(t);
    test_math_matrix_TCRRMV<T, 5, 2, RM, V>(t);
    test_math_matrix_TCRRMV<T, 5, 3, RM, V>(t);
    test_math_matrix_TCRRMV<T, 5, 4, RM, V>(t);
    test_math_matrix_TCRRMV<T, 5, 5, RM, V>(t);
}

template <typename T, class Tester>
void test_math_matrix_T(const Tester& t) {
    test_math_matrix_TRMV<T, true, true>(t);
    test_math_matrix_TRMV<T, true, false>(t);
    test_math_matrix_TRMV<T, false, true>(t);
    test_math_matrix_TRMV<T, false, false>(t);
}

template <class Tester>
void test_math_matrix(const Tester& t) {
    test_math_matrix_T<float>(t);
    test_math_matrix_T<double>(t);
}

struct matrix_default_ctr_tester {
    template <typename T, int C, int R, bool RM, bool V>
    struct _test {
        void operator()() const {
            eagine::math::matrix<T, C, R, RM, V> m;
            BOOST_CHECK_EQUAL(rows(m), R);
            BOOST_CHECK_EQUAL(columns(m), C);
            BOOST_CHECK_EQUAL(row_major(m), RM);
        }
    };

    template <typename T, int C, int R, bool RM, bool V>
    static _test<T, C, R, RM, V> make() {
        return {};
    }
};

BOOST_AUTO_TEST_CASE(math_matrix_default_ctr) {
    matrix_default_ctr_tester t;
    test_math_matrix(t);
}

struct matrix_from1_tester {
    template <typename T, int C, int R, bool RM, bool V>
    struct _test {
        void operator()() const {
            T d[C * R];

            for(int i = 0; i < (C * R); ++i) {
                d[i] = rg.get<T>(-5000, 5000);
            }

            auto m = eagine::math::matrix<T, C, R, RM, V>::from(d, C * R);

            for(int i = 0; i < (RM ? R : C); ++i)
                for(int j = 0; j < (RM ? C : R); ++j) {
                    BOOST_CHECK_EQUAL(m[i][j], d[(RM ? C : R) * i + j]);
                }
        }
    };

    template <typename T, int C, int R, bool RM, bool V>
    static _test<T, C, R, RM, V> make() {
        return {};
    }
};

BOOST_AUTO_TEST_CASE(math_matrix_from1) {
    matrix_from1_tester t;
    test_math_matrix(t);
}

struct matrix_from2_tester {
    template <typename T, int C, int R, bool RM, bool V>
    struct _test {
        template <int I>
        using _uint = std::integral_constant<int, I>;

        template <int M, int N>
        static void _do_test(
          _uint<M>,
          _uint<N>,
          const eagine::math::matrix<T, C, R, RM, V>& m) {
            auto n = eagine::math::matrix<T, M, N, RM, V>::from(m);
            for(int i = 0; i < (RM ? N : M); ++i)
                for(int j = 0; j < (RM ? M : N); ++j) {
                    BOOST_CHECK_EQUAL(n[i][j], m[i][j]);
                }
        }

        template <int N>
        static void _call_test(
          _uint<0>,
          _uint<N>,
          const eagine::math::matrix<T, C, R, RM, V>&) {}

        template <int M>
        static void _call_test(
          _uint<M>,
          _uint<0>,
          const eagine::math::matrix<T, C, R, RM, V>& m) {
            _call_test(_uint<M - 1>(), _uint<R>(), m);
        }

        template <int M, int N>
        static void _call_test(
          _uint<M>,
          _uint<N>,
          const eagine::math::matrix<T, C, R, RM, V>& m) {
            _do_test(_uint<M>(), _uint<N>(), m);
            _call_test(_uint<M>(), _uint<N - 1>(), m);
        }

        void operator()() const {
            T d[C * R];

            for(int i = 0; i < (C * R); ++i) {
                d[i] = rg.get<T>(-5000, 5000);
            }

            auto m = eagine::math::matrix<T, C, R, RM, V>::from(d, C * R);
            _call_test(_uint<C>(), _uint<R>(), m);
        }
    };

    template <typename T, int C, int R, bool RM, bool V>
    static _test<T, C, R, RM, V> make() {
        return {};
    }
};

BOOST_AUTO_TEST_CASE(math_matrix_from2) {
    matrix_from2_tester t;
    test_math_matrix(t);
}

struct matrix_get_set_tester {
    template <typename T, int C, int R, bool RM, bool V>
    struct _test {
        void operator()() const {
            T d[C * R];

            for(int i = 0; i < (C * R); ++i) {
                d[i] = rg.get<T>(-5000, 5000);
            }

            auto m = eagine::math::matrix<T, C, R, RM, V>::from(d, C * R);

            for(int i = 0; i < (RM ? R : C); ++i)
                for(int j = 0; j < (RM ? C : R); ++j) {
                    BOOST_CHECK_EQUAL(m[i][j], d[(RM ? C : R) * i + j]);
                }

            for(int i = 0; i < C; ++i)
                for(int j = 0; j < R; ++j) {
                    set_cm(m, i, j, get_cm(m, i, j) + 1);
                }

            for(int i = 0; i < R; ++i)
                for(int j = 0; j < C; ++j) {
                    BOOST_CHECK_EQUAL(get_rm(m, i, j), get_cm(m, j, i));
                }

            for(int i = 0; i < (RM ? R : C); ++i)
                for(int j = 0; j < (RM ? C : R); ++j) {
                    BOOST_CHECK_EQUAL(m[i][j], d[(RM ? C : R) * i + j] + 1);
                }
        }
    };

    template <typename T, int C, int R, bool RM, bool V>
    static _test<T, C, R, RM, V> make() {
        return {};
    }
};

BOOST_AUTO_TEST_CASE(math_matrix_get_set) {
    matrix_get_set_tester t;
    test_math_matrix(t);
}

struct matrix_transpose_tester {
    template <typename T, int M, int N, bool RM, bool V>
    struct _test {
        void operator()() const {
            T d[M * N];

            for(int i = 0; i < (M * N); ++i) {
                d[i] = rg.get<T>(-5000, 5000);
            }

            auto m = eagine::math::matrix<T, M, N, RM, V>::from(d, M * N);
            eagine::math::matrix<T, N, M, RM, V> n = transpose(m);

            for(int i = 0; i < M; ++i)
                for(int j = 0; j < N; ++j) {
                    BOOST_CHECK_EQUAL(get_cm(m, i, j), get_cm(n, j, i));
                    BOOST_CHECK_EQUAL(get_rm(m, j, i), get_rm(n, i, j));
                }
        }
    };

    template <typename T, int C, int R, bool RM, bool V>
    static _test<T, C, R, RM, V> make() {
        return {};
    }
};

BOOST_AUTO_TEST_CASE(math_matrix_transpose) {
    matrix_transpose_tester t;
    test_math_matrix(t);
}

struct matrix_reorder_tester {
    template <typename T, int M, int N, bool RM, bool V>
    struct _test {
        void operator()() const {
            T d[M * N];

            for(int i = 0; i < (M * N); ++i) {
                d[i] = rg.get<T>(-5000, 5000);
            }

            auto m = eagine::math::matrix<T, M, N, RM, V>::from(d, M * N);
            eagine::math::matrix<T, M, N, !RM, V> n = reorder(m);

            BOOST_CHECK_EQUAL(row_major(m), !row_major(n));
            BOOST_CHECK_EQUAL(row_major(n), !row_major(m));

            for(int i = 0; i < M; ++i)
                for(int j = 0; j < N; ++j) {
                    BOOST_CHECK_EQUAL(get_cm(m, i, j), get_cm(n, i, j));
                    BOOST_CHECK_EQUAL(get_rm(m, j, i), get_rm(n, j, i));
                }
        }
    };

    template <typename T, int C, int R, bool RM, bool V>
    static _test<T, C, R, RM, V> make() {
        return {};
    }
};

BOOST_AUTO_TEST_CASE(math_matrix_reorder) {
    matrix_reorder_tester t;
    test_math_matrix(t);
}

// TODO

BOOST_AUTO_TEST_SUITE_END()

#include "../unit_test_end.inl"
