
/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.types;
import eagine.core.math;
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
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "functions", 2};
    test.once(math_function_is_ppo2_man);
    test.repeat(1000, math_function_is_ppo2_rand);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
