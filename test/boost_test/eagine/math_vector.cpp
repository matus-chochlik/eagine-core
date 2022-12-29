/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/math/vector.hpp>
#define BOOST_TEST_MODULE EAGINE_math_vector
#include "../unit_test_begin.inl"

BOOST_AUTO_TEST_SUITE(math_vector_tests)

static eagine::test_random_generator rg;

template <typename T, int N, bool V>
void test_math_vector_from3_and_elem_TNV() {
    T a[N];
    for(int i = 0; i < N; ++i) {
        a[i] = rg.get_any<T>();
    }

    auto v = eagine::math::vector<T, N, V>::from(a, N);

    for(int i = 0; i < N; ++i) {
        BOOST_CHECK_EQUAL(v[i], a[i]);
    }
}

template <typename T, bool V>
void test_math_vector_from3_and_elem_TV() {
    test_math_vector_from3_and_elem_TNV<T, 1, V>();
    test_math_vector_from3_and_elem_TNV<T, 2, V>();
    test_math_vector_from3_and_elem_TNV<T, 3, V>();
    test_math_vector_from3_and_elem_TNV<T, 4, V>();
    test_math_vector_from3_and_elem_TNV<T, 5, V>();
    test_math_vector_from3_and_elem_TNV<T, 6, V>();
    test_math_vector_from3_and_elem_TNV<T, 7, V>();
    test_math_vector_from3_and_elem_TNV<T, 8, V>();
    test_math_vector_from3_and_elem_TNV<T, 12, V>();
    test_math_vector_from3_and_elem_TNV<T, 15, V>();
    test_math_vector_from3_and_elem_TNV<T, 20, V>();
}

template <typename T>
void test_math_vector_from3_and_elem_T() {
    test_math_vector_from3_and_elem_TV<T, true>();
    test_math_vector_from3_and_elem_TV<T, false>();
}

BOOST_AUTO_TEST_CASE(math_vector_from3_and_elem) {
    test_math_vector_from3_and_elem_T<int>();
    test_math_vector_from3_and_elem_T<float>();
    test_math_vector_from3_and_elem_T<double>();
}

template <typename T, int N, bool V>
void test_math_vector_from4_and_elem_TNV() {
    for(int j = 0; j < N; ++j) {
        T a[N];
        for(int i = 0; i < N - j; ++i) {
            a[i] = rg.get_any<T>();
        }

        T r = rg.get_any<T>();

        auto v = eagine::math::vector<T, N, V>::from(a, N - j, r);

        for(int i = 0; i < N - j; ++i) {
            BOOST_CHECK_EQUAL(v[i], a[i]);
        }

        for(int i = N - j; i < N; ++i) {
            BOOST_CHECK_EQUAL(v[i], r);
        }
    }
}

template <typename T, bool V>
void test_math_vector_from4_and_elem_TV() {
    test_math_vector_from4_and_elem_TNV<T, 1, V>();
    test_math_vector_from4_and_elem_TNV<T, 2, V>();
    test_math_vector_from4_and_elem_TNV<T, 3, V>();
    test_math_vector_from4_and_elem_TNV<T, 4, V>();
    test_math_vector_from4_and_elem_TNV<T, 5, V>();
    test_math_vector_from4_and_elem_TNV<T, 6, V>();
    test_math_vector_from4_and_elem_TNV<T, 7, V>();
    test_math_vector_from4_and_elem_TNV<T, 8, V>();
    test_math_vector_from4_and_elem_TNV<T, 13, V>();
    test_math_vector_from4_and_elem_TNV<T, 16, V>();
    test_math_vector_from4_and_elem_TNV<T, 22, V>();
}

template <typename T>
void test_math_vector_from4_and_elem_T() {
    test_math_vector_from4_and_elem_TV<T, true>();
    test_math_vector_from4_and_elem_TV<T, false>();
}

BOOST_AUTO_TEST_CASE(math_vector_from4_and_elem) {
    test_math_vector_from4_and_elem_T<int>();
    test_math_vector_from4_and_elem_T<float>();
    test_math_vector_from4_and_elem_T<double>();
}

// TODO

BOOST_AUTO_TEST_SUITE_END()

#include "../unit_test_end.inl"
