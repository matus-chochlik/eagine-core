/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.valid_if;
//------------------------------------------------------------------------------
void valid_if_default_construct_char(auto& s) {
    eagitest::case_ test{s, 1, "default construct char"};

    eagine::valid_if_one_of<char, 'A', 'B', 'C'> v;

    test.check(not v.has_value(), "has not value");
    test.check(not v, "is false");
    test.check(not bool(v), "is not true");

    test.check(
      not v.and_then([](char) { return false; }).has_value(), "and then");
}
//------------------------------------------------------------------------------
void valid_if_default_construct_int(auto& s) {
    eagitest::case_ test{s, 2, "default construct int"};

    eagine::valid_if_one_of<int, 1, 2, 3> v;

    test.check(not v.has_value(), "has not value");
    test.check(not v, "is false");
    test.check(not bool(v), "is not true");

    test.check(
      not v.and_then([](int) { return true; }).has_value(), "and then");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "valid_if range", 2};
    test.once(valid_if_default_construct_char);
    test.once(valid_if_default_construct_int);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
