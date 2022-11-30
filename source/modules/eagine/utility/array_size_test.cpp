/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.utility;
//------------------------------------------------------------------------------
void array_size(auto& s) {
    eagitest::case_ test{s, 1, "result"};

    float f3[] = {1.f, 2.f, 3.f};
    int i5[] = {1, 2, 3, 4, 5};
    int i9[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    const char* s7[] = {"A", "B", "C", "D", "E", "F", "G"};

    static_assert(eagine::array_size(f3) == 3);
    test.check(eagine::array_size(f3) == 3, "f3");

    static_assert(eagine::array_size(i5) == 5);
    test.check(eagine::array_size(i5) == 5, "i5");

    static_assert(eagine::array_size(s7) == 7);
    test.check(eagine::array_size(s7) == 7, "s7");

    static_assert(eagine::array_size(i9) == 9);
    test.check(eagine::array_size(i9) == 9, "f9");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "array_size", 1};
    test.once(array_size);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
