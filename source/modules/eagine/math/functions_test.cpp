
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
// PPOT
//------------------------------------------------------------------------------
template <typename T>
void math_function_is_ppo2_T(eagitest::case_& test, const T v) {
    using std::pow;

    const bool r1 = eagine::math::is_positive_power_of_2(v);
    bool r2 = false;
    T b = 0;
    while(T(pow(T(2), b)) <= v) {
        if(T(pow(T(2), b)) == v) {
            r2 = true;
            break;
        }
        ++b;
    }

    test.check_equal(r1, r2, "equal");
}
//------------------------------------------------------------------------------
void math_function_is_ppo2_man(auto& s) {
    eagitest::case_ test{s, 1, "positive power of 2 manual"};

    for(auto i : {0, 1, 2, 3, 5, 7, 9, 16, 17, 31, 32, 64, 127, 128, 129}) {
        math_function_is_ppo2_T(test, short(i));
        math_function_is_ppo2_T(test, int(i));
        math_function_is_ppo2_T(test, long(i));
        math_function_is_ppo2_T(test, unsigned(i));
    }
}
//------------------------------------------------------------------------------
void math_function_is_ppo2_rand(unsigned, auto& s) {
    eagitest::case_ test{s, 2, "positive power of 2 random"};
    auto& rg{test.random()};

    math_function_is_ppo2_T(test, rg.get_between<short>(0, 1000));
    math_function_is_ppo2_T(test, rg.get_between<int>(0, 1000));
    math_function_is_ppo2_T(test, rg.get_between<long>(0, 10000));
    math_function_is_ppo2_T(test, rg.get_between<unsigned>(0, 1000));
}
//------------------------------------------------------------------------------
// GCD
//------------------------------------------------------------------------------
template <typename T>
void math_function_gcd_T(eagitest::case_& test, const T l, const T r) {
    const T d = eagine::math::greatest_common_divisor(l, r);

    test.check_equal(l % d, T(0), "l");
    test.check_equal(r % d, T(0), "r");
}
//------------------------------------------------------------------------------
void math_function_gcd(unsigned, auto& s) {
    eagitest::case_ test{s, 3, "greatest common divisor"};
    auto& rg{test.random()};

    math_function_gcd_T(test, rg.get_any<short>(), rg.get_any<short>());
    math_function_gcd_T(test, rg.get_any<int>(), rg.get_any<int>());
    math_function_gcd_T(test, rg.get_any<long>(), rg.get_any<long>());
    math_function_gcd_T(test, rg.get_any<unsigned>(), rg.get_any<unsigned>());
}
//------------------------------------------------------------------------------
// min/max
//------------------------------------------------------------------------------
template <typename T, typename... P>
void math_function_min_max_TP(eagitest::case_& test, P... v) {
    T a[sizeof...(P)] = {v...};

    using namespace eagine;

    T minv = math::minimum(v...);
    T maxv = math::maximum(v...);

    for(T x : a) {
        test.check(x >= minv, "min");
        test.check(x <= maxv, "max");
    }
}
//------------------------------------------------------------------------------
template <typename T>
void math_function_min_max_T(eagitest::case_& test) {
    const auto rndval = [&]() {
        return test.random().get_any<T>();
    };

    math_function_min_max_TP<T>(test, rndval(), rndval());
    math_function_min_max_TP<T>(test, rndval(), rndval(), rndval());
    math_function_min_max_TP<T>(test, rndval(), rndval(), rndval(), rndval());
    math_function_min_max_TP<T>(
      test, rndval(), rndval(), rndval(), rndval(), rndval());
    math_function_min_max_TP<T>(
      test,
      rndval(),
      rndval(),
      rndval(),
      rndval(),
      rndval(),
      rndval(),
      rndval(),
      rndval(),
      rndval(),
      rndval(),
      rndval(),
      rndval());
}
//------------------------------------------------------------------------------
void math_function_min_max(unsigned, auto& s) {
    eagitest::case_ test{s, 4, "minimum & maximum"};
    math_function_min_max_T<short>(test);
    math_function_min_max_T<int>(test);
    math_function_min_max_T<float>(test);
    math_function_min_max_T<double>(test);
}
//------------------------------------------------------------------------------
// factorial
//------------------------------------------------------------------------------
void math_function_factorial(auto& s) {
    eagitest::case_ test{s, 5, "factorial"};
    using namespace eagine::math;

    test.check_equal(factorial(-2), 1, "A");
    test.check_equal(factorial(-1), 1, "B");

    test.check_equal(factorial(0), 1, "C");
    test.check_equal(factorial(1), 1, "D");
    test.check_equal(factorial(2), 2, "E");
    test.check_equal(factorial(3), 6, "F");
    test.check_equal(factorial(4), 24, "G");
    test.check_equal(factorial(5), 120, "H");
    test.check_equal(factorial(6), 720, "I");
    test.check_equal(factorial(7), 5040, "J");

    test.check_equal(factorial(12), 479001600, "K");
}
//------------------------------------------------------------------------------
// binomial
//------------------------------------------------------------------------------
void math_function_binomial(auto& s) {
    eagitest::case_ test{s, 6, "binomial"};
    using namespace eagine::math;

    test.check_equal(binomial(-1, 0), 0, "A");

    test.check_equal(binomial(0, 0), 1, "B");

    for(int n = 1; n < 12; ++n) {
        test.check_equal(binomial(n, -1), 0, "C");
        test.check_equal(binomial(n, 0), 1, "D");

        for(int k = 1; k < n; ++k) {
            test.check_equal(
              binomial(n, k), binomial(n - 1, k - 1) + binomial(n - 1, k), "E");
        }

        test.check_equal(binomial(n, n), 1, "F");
        test.check_equal(binomial(n, n + 1), 0, "G");
    }
}
//------------------------------------------------------------------------------
// bezier_point
//------------------------------------------------------------------------------
template <typename T>
void math_functions_bezier_point_T_1(eagitest::case_& test) {
    using eagine::math::bezier_point;
    auto& rg{test.random()};

    T t = rg.get_between<T>(0, 1);
    T v0 = rg.get_between<T>(-1000, 1000);

    test.check_close(bezier_point(t, v0), v0, "1");
}
//------------------------------------------------------------------------------
template <typename T>
void math_functions_bezier_point_T_2(eagitest::case_& test) {
    using eagine::math::bezier_point;
    auto& rg{test.random()};

    T t = rg.get_between<T>(0, 1);
    T v0 = rg.get_between<T>(-1000, 1000);
    T v1 = rg.get_between<T>(-1000, 1000);

    test.check_close(bezier_point(t, v0, v1), (1 - t) * v0 + t * v1, "2");
}
//------------------------------------------------------------------------------
template <typename T>
void math_functions_bezier_point_T_3(eagitest::case_& test) {
    using eagine::math::bezier_point;
    auto& rg{test.random()};

    T t = rg.get_between<T>(0, 1);
    T v0 = rg.get_between<T>(-1000, 1000);
    T v1 = rg.get_between<T>(-1000, 1000);
    T v2 = rg.get_between<T>(-1000, 1000);

    test.check_close(
      bezier_point(t, v0, v1, v2),
      (1 - t) * (1 - t) * v0 + 2 * (1 - t) * t * v1 + t * t * v2,
      "3");
}
//------------------------------------------------------------------------------
template <typename T>
void math_functions_bezier_point_T_4(eagitest::case_& test) {
    using eagine::math::bezier_point;
    auto& rg{test.random()};

    T t = rg.get_between<T>(0, 1);
    T v0 = rg.get_between<T>(-1000, 1000);
    T v1 = rg.get_between<T>(-1000, 1000);
    T v2 = rg.get_between<T>(-1000, 1000);
    T v3 = rg.get_between<T>(-1000, 1000);

    test.check_close(
      bezier_point(t, v0, v1, v2, v3),
      (1 - t) * (1 - t) * (1 - t) * v0 + 3 * (1 - t) * (1 - t) * t * v1 +
        3 * (1 - t) * t * t * v2 + t * t * t * v3,
      "4");
}
//------------------------------------------------------------------------------
template <typename T>
void math_functions_bezier_point_T_5(eagitest::case_& test) {
    using eagine::math::bezier_point;
    auto& rg{test.random()};

    T t = rg.get_between<T>(0, 1);
    T v0 = rg.get_between<T>(-1000, 1000);
    T v1 = rg.get_between<T>(-1000, 1000);
    T v2 = rg.get_between<T>(-1000, 1000);
    T v3 = rg.get_between<T>(-1000, 1000);
    T v4 = rg.get_between<T>(-1000, 1000);

    test.check_close(
      bezier_point(t, v0, v1, v2, v3, v4),
      (1 - t) * (1 - t) * (1 - t) * (1 - t) * v0 +
        4 * (1 - t) * (1 - t) * (1 - t) * t * v1 +
        6 * (1 - t) * (1 - t) * t * t * v2 + 4 * (1 - t) * t * t * t * v3 +
        t * t * t * t * v4,
      "5");
}
//------------------------------------------------------------------------------
template <typename T>
void math_functions_bezier_point_T(eagitest::case_& test) {
    math_functions_bezier_point_T_1<T>(test);
    math_functions_bezier_point_T_2<T>(test);
    math_functions_bezier_point_T_3<T>(test);
    math_functions_bezier_point_T_4<T>(test);
    math_functions_bezier_point_T_5<T>(test);
}
//------------------------------------------------------------------------------
void math_function_bezier_point(auto& s) {
    eagitest::case_ test{s, 7, "bezier point"};

    math_functions_bezier_point_T<float>(test);
    math_functions_bezier_point_T<double>(test);
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "functions", 7};
    test.once(math_function_is_ppo2_man);
    test.repeat(1000, math_function_is_ppo2_rand);
    test.repeat(1000, math_function_gcd);
    test.repeat(1000, math_function_min_max);
    test.once(math_function_factorial);
    test.once(math_function_binomial);
    test.once(math_function_bezier_point);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
