/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/vect/from.hpp>
#define BOOST_TEST_MODULE EAGINE_vect_from
#include "../unit_test_begin.inl"

BOOST_AUTO_TEST_SUITE(vect_from_tests)

static eagine::test_random_generator rg;

template <typename T, int N, bool V>
void test_vect_from_array_TNV() {
    T a[N];
    for(int i = 0; i < N; ++i) {
        a[i] = rg.get_any<T>();
    }

    typename eagine::vect::data<T, N, V>::type v =
      eagine::vect::from_array<T, N, V>::apply(a, N);

    for(int i = 0; i < N; ++i) {
        BOOST_CHECK_EQUAL(v[i], a[i]);
    }
}

template <typename T, bool V>
void test_vect_from_array_TV() {
    test_vect_from_array_TNV<T, 2, V>();
    test_vect_from_array_TNV<T, 3, V>();
    test_vect_from_array_TNV<T, 4, V>();
    test_vect_from_array_TNV<T, 5, V>();
    test_vect_from_array_TNV<T, 6, V>();
    test_vect_from_array_TNV<T, 7, V>();
    test_vect_from_array_TNV<T, 8, V>();
}

template <typename T>
void test_vect_from_array_T() {
    test_vect_from_array_TV<T, true>();
    test_vect_from_array_TV<T, false>();
}

BOOST_AUTO_TEST_CASE(vect_from_array) {
    for(int k = 0; k < 1000; ++k) {
        test_vect_from_array_T<int>();
        test_vect_from_array_T<float>();
        test_vect_from_array_T<double>();
    }
}

template <typename T, int N, int M, bool V>
void test_vect_from_saafv_TNMV() {
    static_assert(M <= N, "");

    T a[M];
    for(int i = 0; i < M; ++i) {
        a[i] = rg.get_any<T>();
    }
    T f = rg.get_any<T>();

    typename eagine::vect::data<T, N, V>::type v =
      eagine::vect::from_saafv<T, N, V>::apply(a, M, f);

    for(int i = 0; i < M; ++i) {
        BOOST_CHECK_EQUAL(v[i], a[i]);
    }
    for(int i = M; i < N; ++i) {
        BOOST_CHECK_EQUAL(v[i], f);
    }
}

template <typename T, bool V>
void test_vect_from_saafv_TV() {
    test_vect_from_saafv_TNMV<T, 2, 1, V>();
    test_vect_from_saafv_TNMV<T, 2, 2, V>();

    test_vect_from_saafv_TNMV<T, 3, 1, V>();
    test_vect_from_saafv_TNMV<T, 3, 2, V>();
    test_vect_from_saafv_TNMV<T, 3, 3, V>();

    test_vect_from_saafv_TNMV<T, 4, 1, V>();
    test_vect_from_saafv_TNMV<T, 4, 2, V>();
    test_vect_from_saafv_TNMV<T, 4, 3, V>();
    test_vect_from_saafv_TNMV<T, 4, 4, V>();

    test_vect_from_saafv_TNMV<T, 5, 1, V>();
    test_vect_from_saafv_TNMV<T, 5, 2, V>();
    test_vect_from_saafv_TNMV<T, 5, 3, V>();
    test_vect_from_saafv_TNMV<T, 5, 4, V>();
    test_vect_from_saafv_TNMV<T, 5, 5, V>();

    test_vect_from_saafv_TNMV<T, 6, 1, V>();
    test_vect_from_saafv_TNMV<T, 6, 2, V>();
    test_vect_from_saafv_TNMV<T, 6, 3, V>();
    test_vect_from_saafv_TNMV<T, 6, 4, V>();
    test_vect_from_saafv_TNMV<T, 6, 5, V>();
    test_vect_from_saafv_TNMV<T, 6, 6, V>();

    test_vect_from_saafv_TNMV<T, 7, 1, V>();
    test_vect_from_saafv_TNMV<T, 7, 2, V>();
    test_vect_from_saafv_TNMV<T, 7, 3, V>();
    test_vect_from_saafv_TNMV<T, 7, 4, V>();
    test_vect_from_saafv_TNMV<T, 7, 5, V>();
    test_vect_from_saafv_TNMV<T, 7, 6, V>();
    test_vect_from_saafv_TNMV<T, 7, 7, V>();

    test_vect_from_saafv_TNMV<T, 8, 1, V>();
    test_vect_from_saafv_TNMV<T, 8, 2, V>();
    test_vect_from_saafv_TNMV<T, 8, 3, V>();
    test_vect_from_saafv_TNMV<T, 8, 4, V>();
    test_vect_from_saafv_TNMV<T, 8, 5, V>();
    test_vect_from_saafv_TNMV<T, 8, 6, V>();
    test_vect_from_saafv_TNMV<T, 8, 7, V>();
    test_vect_from_saafv_TNMV<T, 8, 8, V>();
}

template <typename T>
void test_vect_from_saafv_T() {
    test_vect_from_saafv_TV<T, true>();
    test_vect_from_saafv_TV<T, false>();
}

BOOST_AUTO_TEST_CASE(vect_from_saafv) {
    for(int k = 0; k < 1000; ++k) {
        test_vect_from_saafv_T<int>();
        test_vect_from_saafv_T<float>();
        test_vect_from_saafv_T<double>();
    }
}

BOOST_AUTO_TEST_SUITE_END()

#include "../unit_test_end.inl"
