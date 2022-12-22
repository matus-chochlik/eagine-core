/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/math/functions.hpp>
#define BOOST_TEST_MODULE EAGINE_math_functions
#include "../unit_test_begin.inl"

BOOST_AUTO_TEST_SUITE(math_function_tests)

static eagine::test_random_generator rg;

template <typename T>
void test_math_functions_bezier_1(int l) {
    using eagine::math::bezier_point;

    for(int i = 0; i < l; ++i) {
        T t = rg.get<T>(0, 1);
        T x = rg.get<T>(-1000, 1000);

        BOOST_CHECK_EQUAL(bezier_point(t, x), x);
    }
}

template <typename T>
void test_math_functions_bezier_2(int l) {
    using eagine::math::bezier_point;

    for(int i = 0; i < l; ++i) {
        T t = rg.get<T>(0, 1);
        T x = rg.get<T>(-1000, 1000);
        T y = rg.get<T>(-1000, 1000);

        BOOST_CHECK_EQUAL(bezier_point(t, x, y), (1 - t) * x + t * y);
    }
}

template <typename T>
void test_math_functions_bezier_3(int l) {
    using eagine::math::bezier_point;

    for(int i = 0; i < l; ++i) {
        T t = rg.get<T>(0, 1);
        T x = rg.get<T>(-1000, 1000);
        T y = rg.get<T>(-1000, 1000);
        T z = rg.get<T>(-1000, 1000);

        BOOST_CHECK_CLOSE(
          bezier_point(t, x, y, z),
          (1 - t) * (1 - t) * x + 2 * (1 - t) * t * y + t * t * z,
          0.06);
    }
}

template <typename T>
void test_math_functions_bezier_4(int l) {
    using eagine::math::bezier_point;

    for(int i = 0; i < l; ++i) {
        T t = rg.get<T>(0, 1);
        T v0 = rg.get<T>(-1000, 1000);
        T v1 = rg.get<T>(-1000, 1000);
        T v2 = rg.get<T>(-1000, 1000);
        T v3 = rg.get<T>(-1000, 1000);

        BOOST_CHECK_CLOSE(
          bezier_point(t, v0, v1, v2, v3),
          (1 - t) * (1 - t) * (1 - t) * v0 + 3 * (1 - t) * (1 - t) * t * v1 +
            3 * (1 - t) * t * t * v2 + t * t * t * v3,
          0.6);
    }
}

template <typename T>
void test_math_functions_bezier_5(int l) {
    using eagine::math::bezier_point;

    for(int i = 0; i < l; ++i) {
        T t = rg.get<T>(0, 1);
        T v0 = rg.get<T>(-1000, 1000);
        T v1 = rg.get<T>(-1000, 1000);
        T v2 = rg.get<T>(-1000, 1000);
        T v3 = rg.get<T>(-1000, 1000);
        T v4 = rg.get<T>(-1000, 1000);

        BOOST_CHECK_CLOSE(
          bezier_point(t, v0, v1, v2, v3, v4),
          (1 - t) * (1 - t) * (1 - t) * (1 - t) * v0 +
            4 * (1 - t) * (1 - t) * (1 - t) * t * v1 +
            6 * (1 - t) * (1 - t) * t * t * v2 + 4 * (1 - t) * t * t * t * v3 +
            t * t * t * t * v4,
          0.6);
    }
}

template <typename T>
void test_math_functions_bezier(int l) {
    test_math_functions_bezier_1<T>(l);
    test_math_functions_bezier_2<T>(l);
    test_math_functions_bezier_3<T>(l);
    test_math_functions_bezier_4<T>(l);
    test_math_functions_bezier_5<T>(l);
}

BOOST_AUTO_TEST_CASE(math_functions_bezier) {
    test_math_functions_bezier<float>(100);
    test_math_functions_bezier<double>(100);
}

// TODO

BOOST_AUTO_TEST_SUITE_END()

#include "../unit_test_end.inl"
