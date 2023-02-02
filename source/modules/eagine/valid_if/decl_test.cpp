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
void valid_if_default_construct(auto& s) {
    eagitest::case_ test{s, 1, "default construct"};

    eagine::optionally_valid<int> v;

    test.check(not v.has_value(), "has not value");
    test.check(not v, "is false");
    test.check(not bool(v), "is not true");

    test.check(
      not v.and_then([](int) { return true; }).has_value(), "and then");
}
//------------------------------------------------------------------------------
void valid_if_non_ref(auto& s) {
    eagitest::case_ test{s, 2, "non-reference"};

    eagine::optionally_valid<int> v{123, true};

    test.check(bool(v), "is true");
    test.check(not not v, "is not false");
    test.ensure(v.has_value(), "has value");
    test.check_equal(v.value(), 123, "value ok");
}
//------------------------------------------------------------------------------
void valid_if_ref(auto& s) {
    eagitest::case_ test{s, 3, "reference"};

    int i = 234;
    eagine::optionally_valid<int&> v{i, true};

    test.check(bool(v), "is true");
    test.check(not not v, "is not false");
    test.ensure(v.has_value(), "has value");
    test.check_equal(v.value(), 234, "value 234");

    i = 456;

    test.ensure(v.has_value(), "has value");
    test.check_equal(v.value(), 456, "value 456");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "valid_if", 3};
    test.once(valid_if_default_construct);
    test.once(valid_if_non_ref);
    test.once(valid_if_ref);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
