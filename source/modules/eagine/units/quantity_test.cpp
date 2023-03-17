/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.units;
//------------------------------------------------------------------------------
// quantity 1
//------------------------------------------------------------------------------
template <typename T>
void quantity_1(eagitest::case_& test) {
    auto& rg{test.random()};
    using namespace eagine;

    const T r1 = rg.get_between<T>(-1000, 1000);
    const T r2 = rg.get_between<T>(-1000, 1000);

    degrees_t<T> v1 = degrees_(r1);
    degrees_t<T> v2 = degrees_(r2);
    degrees_t<T> v3 = v1 + v2;
    degrees_t<T> v4 = v1 - v2;

    test.check(not(T(v1) > r1), "A");
    test.check(not(T(v1) < r1), "B");
    test.check(not(T(v2) > r2), "C");
    test.check(not(T(v2) < r2), "D");
    test.check(not(T(v3) > r1 + r2), "E");
    test.check(not(T(v3) < r1 + r2), "F");
    test.check(not(T(v4) > r1 - r2), "G");
    test.check(not(T(v4) < r1 - r2), "H");

    test.check(not(T(+v1) > +r1), "I");
    test.check(not(T(+v1) < +r1), "J");
    test.check(not(T(-v2) > -r2), "K");
    test.check(not(T(-v2) < -r2), "L");

    test.check_equal(v1 > v2, r1 > r2, "M");
    test.check_equal(v1 < v2, r1 < r2, "N");
    test.check_equal(v1 >= v2, r1 >= r2, "O");
    test.check_equal(v1 <= v2, r1 <= r2, "P");
}
//------------------------------------------------------------------------------
void units_quantity_1(auto& s) {
    eagitest::case_ test{s, 1, "1"};
    quantity_1<int>(test);
    quantity_1<float>(test);
    quantity_1<double>(test);
}
//------------------------------------------------------------------------------
// angles 1
//------------------------------------------------------------------------------
template <typename T>
void angles_1(eagitest::case_& test) {
    using namespace eagine;

    test.check_close(sin(radians_(T(0))), T(0), "A");
    test.check_close(sin(degrees_(T(0))), T(0), "B");
    test.check_close(sin(degrees_(T(90))), T(1), "C");
    test.check_close(sin(degrees_(T(180))), T(0), "D");
    test.check_close(sin(degrees_(T(270))), T(-1), "E");
    test.check_close(sin(degrees_(T(360))), T(0), "F");

    test.check_close(cos(radians_(T(0))), T(1), "G");
    test.check_close(cos(degrees_(T(0))), T(1), "H");
    test.check_close(cos(degrees_(T(90))), T(0), "I");
    test.check_close(cos(degrees_(T(180))), T(-1), "J");
    test.check_close(cos(degrees_(T(270))), T(0), "K");
    test.check_close(cos(degrees_(T(360))), T(1), "L");
}
//------------------------------------------------------------------------------
void units_angles_1(auto& s) {
    eagitest::case_ test{s, 2, " angles 1"};
    angles_1<float>(test);
    angles_1<double>(test);
}
//------------------------------------------------------------------------------
// angles 2
//------------------------------------------------------------------------------
template <typename T>
void angles_2(eagitest::case_& test) {
    auto& rg{test.random()};
    using namespace eagine;

    for(unsigned i = 0; i < test.repeats(1000); ++i) {
        T x = rg.get_between<T>(-1000, 1000);

        test.check_close(sin(radians_(x)), std::sin(x), "sin");
        test.check_close(cos(radians_(x)), std::cos(x), "cos");
        test.check_close(tan(radians_(x)), std::tan(x), "tan");
    }
}
//------------------------------------------------------------------------------
void units_angles_2(auto& s) {
    eagitest::case_ test{s, 3, " angles 2"};
    angles_2<float>(test);
    angles_2<double>(test);
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "quantity", 3};
    test.once(units_quantity_1);
    test.once(units_angles_1);
    test.once(units_angles_2);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
