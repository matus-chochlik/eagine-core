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

template <typename T, bool V>
void test_math_vector_from2_and_elem_TV() {
    T ra = rg.get_any<T>();
    auto v1a = eagine::math::vector<T, 1, V>::make(ra);
    BOOST_CHECK_EQUAL(v1a[0], ra);

    T rb = rg.get_any<T>();
    auto v1b = eagine::math::vector<T, 1, V>::make(rb);
    BOOST_CHECK_EQUAL(v1b[0], rb);

    auto v2aa = eagine::math::vector<T, 2, V>::from(v1a, v1a);
    BOOST_CHECK_EQUAL(v2aa[0], ra);
    BOOST_CHECK_EQUAL(v2aa[1], ra);

    auto v2ab = eagine::math::vector<T, 2, V>::from(v1a, v1b);
    BOOST_CHECK_EQUAL(v2ab[0], ra);
    BOOST_CHECK_EQUAL(v2ab[1], rb);

    auto v2bb = eagine::math::vector<T, 2, V>::from(v1b, v1b);
    BOOST_CHECK_EQUAL(v2bb[0], rb);
    BOOST_CHECK_EQUAL(v2bb[1], rb);

    auto v3aba = eagine::math::vector<T, 3, V>::from(v2ab, v1a);
    BOOST_CHECK_EQUAL(v3aba[0], ra);
    BOOST_CHECK_EQUAL(v3aba[1], rb);
    BOOST_CHECK_EQUAL(v3aba[2], ra);

    auto v3bab = eagine::math::vector<T, 3, V>::from(v1b, v2ab);
    BOOST_CHECK_EQUAL(v3bab[0], rb);
    BOOST_CHECK_EQUAL(v3bab[1], ra);
    BOOST_CHECK_EQUAL(v3bab[2], rb);

    auto v4abab = eagine::math::vector<T, 4, V>::from(v1a, v3bab);
    BOOST_CHECK_EQUAL(v4abab[0], ra);
    BOOST_CHECK_EQUAL(v4abab[1], rb);
    BOOST_CHECK_EQUAL(v4abab[2], ra);
    BOOST_CHECK_EQUAL(v4abab[3], rb);

    auto v4aabb = eagine::math::vector<T, 4, V>::from(v2aa, v2bb);
    BOOST_CHECK_EQUAL(v4aabb[0], ra);
    BOOST_CHECK_EQUAL(v4aabb[1], ra);
    BOOST_CHECK_EQUAL(v4aabb[2], rb);
    BOOST_CHECK_EQUAL(v4aabb[3], rb);

    auto v4baba = eagine::math::vector<T, 4, V>::from(v3bab, v1a);
    BOOST_CHECK_EQUAL(v4baba[0], rb);
    BOOST_CHECK_EQUAL(v4baba[1], ra);
    BOOST_CHECK_EQUAL(v4baba[2], rb);
    BOOST_CHECK_EQUAL(v4baba[3], ra);

    auto v5ababa = eagine::math::vector<T, 5, V>::from(v1a, v4baba);
    BOOST_CHECK_EQUAL(v5ababa[0], ra);
    BOOST_CHECK_EQUAL(v5ababa[1], rb);
    BOOST_CHECK_EQUAL(v5ababa[2], ra);
    BOOST_CHECK_EQUAL(v5ababa[3], rb);
    BOOST_CHECK_EQUAL(v5ababa[4], ra);

    auto v5aabab = eagine::math::vector<T, 5, V>::from(v2aa, v3bab);
    BOOST_CHECK_EQUAL(v5aabab[0], ra);
    BOOST_CHECK_EQUAL(v5aabab[1], ra);
    BOOST_CHECK_EQUAL(v5aabab[2], rb);
    BOOST_CHECK_EQUAL(v5aabab[3], ra);
    BOOST_CHECK_EQUAL(v5aabab[4], rb);

    auto v5ababb = eagine::math::vector<T, 5, V>::from(v3aba, v2bb);
    BOOST_CHECK_EQUAL(v5ababb[0], ra);
    BOOST_CHECK_EQUAL(v5ababb[1], rb);
    BOOST_CHECK_EQUAL(v5ababb[2], ra);
    BOOST_CHECK_EQUAL(v5ababb[3], rb);
    BOOST_CHECK_EQUAL(v5ababb[4], rb);

    auto v5babab = eagine::math::vector<T, 5, V>::from(v4baba, v1b);
    BOOST_CHECK_EQUAL(v5babab[0], rb);
    BOOST_CHECK_EQUAL(v5babab[1], ra);
    BOOST_CHECK_EQUAL(v5babab[2], rb);
    BOOST_CHECK_EQUAL(v5babab[3], ra);
    BOOST_CHECK_EQUAL(v5babab[4], rb);
}

template <typename T>
void test_math_vector_from2_and_elem_T() {
    test_math_vector_from2_and_elem_TV<T, true>();
    test_math_vector_from2_and_elem_TV<T, false>();
}

BOOST_AUTO_TEST_CASE(math_vector_from2_and_elem) {
    test_math_vector_from2_and_elem_T<int>();
    test_math_vector_from2_and_elem_T<float>();
    test_math_vector_from2_and_elem_T<double>();
}

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
