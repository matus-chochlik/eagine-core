/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import std;
import eagine.core.utility;
//------------------------------------------------------------------------------
void overloaded_1(auto& s) {
    eagitest::case_ test{s, 1, "1"};
    const auto func = eagine::overloaded(
      [](int) -> std::string_view { return "int"; },
      [](float) -> std::string_view { return "float"; },
      [](double) -> std::string_view { return "double"; },
      [](auto) -> std::string_view { return "other"; });

    test.check_equal(func(100), "int", "int");
    test.check_equal(func(2.f), "float", "float");
    test.check_equal(func(3.0), "double", "double");
    test.check_equal(func("x"), "other", "double");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "overloaded", 1};
    test.once(overloaded_1);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
