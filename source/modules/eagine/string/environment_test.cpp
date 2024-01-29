/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
//------------------------------------------------------------------------------
void environment_get(auto& s) {
    using eagine::get_environment_variable;
    eagitest::case_ test{s, 1, "get"};

    const auto ev0{get_environment_variable("EAGINE_NO_SUCH_VARIABLE")};
    test.check(not ev0.has_value(), "has not value");
    test.check_equal(ev0.value_or("qux"), "qux", "value 0");

    const auto ev1{get_environment_variable("EAGINE_TEST_VAR1")};
    test.check(ev1.has_value(), "has value 1");
    test.check_equal(ev1.value_or("foo"), "eagine", "value 1");

    const auto ev2{get_environment_variable("EAGINE_TEST_VAR2")};
    test.check(ev2.has_value(), "has value 2");
    test.check_equal(ev2.value_or("bar"), "core", "value 2");

    const auto ev3{get_environment_variable("EAGINE_TEST_VAR3")};
    test.check(ev3.has_value(), "has value 3");
    test.check_equal(ev3.value_or("baz"), "string", "value 3");
}
//------------------------------------------------------------------------------
void environment_get_placeholder(auto& s) {
    using eagine::_1;
    using eagine::_2;
    using eagine::get_environment_variable;
    eagitest::case_ test{s, 2, "get placeholder"};

    const auto n0{"EAGINE_NO_SUCH_VARIABLE"};
    auto ev0{get_environment_variable(_1)};
    test.check(not ev0(n0).has_value(), "has not value");
    test.check_equal(ev0(n0).value_or("foo"), "foo", "value 0");

    const auto n1{"EAGINE_TEST_VAR1"};
    auto ev1{get_environment_variable(_1)};
    test.check(ev1(n1).has_value(), "has value 1");
    test.check_equal(ev1(n1).value_or("bar"), "eagine", "value 1");

    const auto n2{"EAGINE_TEST_VAR2"};
    auto ev2{get_environment_variable(_1)};
    test.check(ev2(n2, 123).has_value(), "has value 2");
    test.check_equal(ev2(n2, 123).value_or("baz"), "core", "value 2");

    const auto n3{"EAGINE_TEST_VAR3"};
    auto ev3{get_environment_variable(_2)};
    test.check(ev3(234, n3).has_value(), "has value 3");
    test.check_equal(ev3(234, n3).value_or("qux"), "string", "value 3");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "environment", 2};
    test.once(environment_get);
    test.once(environment_get_placeholder);

    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
