/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/vect/hsum.hpp>
#define BOOST_TEST_MODULE eagine_vect_hsum
#include "../unit_test_begin.inl"

BOOST_AUTO_TEST_SUITE(vect_hsum_tests)

static eagine::test_random_generator rg;

template <typename T, int N, bool V>
void test_vect_hsum_TNV() {
    for(int k = 0; k < 10; ++k) {
        T s = T(0);

        typename eagine::vect::data<T, N, V>::type v = {};

        for(int i = 0; i < N; ++i) {
            T n = rg.get<T>(0, 5000);
            v[i] = n;
            s += n;
        }

        typename eagine::vect::data<T, N, V>::type r =
          eagine::vect::hsum<T, N, V>::apply(v);

        for(int i = 0; i < N; ++i) {
            BOOST_CHECK_EQUAL(s, r[i]);
        }
    }
}

template <typename T, bool V>
void test_vect_hsum_TV() {
    test_vect_hsum_TNV<T, 2, V>();
    test_vect_hsum_TNV<T, 3, V>();
    test_vect_hsum_TNV<T, 4, V>();
    test_vect_hsum_TNV<T, 5, V>();
    test_vect_hsum_TNV<T, 6, V>();
    test_vect_hsum_TNV<T, 7, V>();
    test_vect_hsum_TNV<T, 8, V>();

    test_vect_hsum_TNV<T, 11, V>();
    test_vect_hsum_TNV<T, 17, V>();
    test_vect_hsum_TNV<T, 23, V>();
}

template <typename T>
void test_vect_hsum_T() {
    test_vect_hsum_TV<T, true>();
    test_vect_hsum_TV<T, false>();
}

BOOST_AUTO_TEST_CASE(vect_hsum_apply_int) {
    test_vect_hsum_T<int>();
}

BOOST_AUTO_TEST_CASE(vect_hsum_apply_float) {
    test_vect_hsum_T<float>();
}

BOOST_AUTO_TEST_CASE(vect_hsum_apply_double) {
    test_vect_hsum_T<double>();
}

BOOST_AUTO_TEST_SUITE_END()

#include "../unit_test_end.inl"
