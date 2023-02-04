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
struct test_person {
    std::string given_name;
    std::string family_name;
};
//------------------------------------------------------------------------------
void valid_if_default_construct(auto& s) {
    eagitest::case_ test{s, 1, "default construct"};

    eagine::optionally_valid<test_person> v;

    test.check(not v.has_value(), "has not value");
    test.check(not v, "is false");
    test.check(not bool(v), "is not true");

    test.check(
      not v.and_then([](const test_person&) { return true; }).has_value(),
      "and then");
    test.check(
      not v.member(&test_person::given_name).has_value(), "and then member");
}
//------------------------------------------------------------------------------
void valid_if_non_ref(auto& s) {
    eagitest::case_ test{s, 2, "non-reference"};

    eagine::optionally_valid<int> v{123, true};

    test.check(bool(v), "is true");
    test.check(not not v, "is not false");
    test.ensure(v.has_value(), "has value");
    test.check_equal(v.value(), 123, "value ok");

    const auto f{[](int i) {
        return i * 2;
    }};
    test.ensure(v.and_then(f).has_value(), "and then has value");
    test.check_equal(v.and_then(f).value(), 246, "and then value ok");

    eagine::always_valid<test_person> p{{"Jane", "Doe"}};
    test.check(bool(p), "is true");
    test.check(not not p, "is not false");
    test.ensure(p.has_value(), "has value");
    test.check_equal(
      p.member(&test_person::family_name).value(),
      "Doe",
      "and then member value");
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

    const auto f{[](const int& j) {
        return j * 2;
    }};
    test.check(v.and_then(f).has_value(), "and then has value");
    test.check_equal(v.and_then(f).value(), i * 2, "and then value ok");

    eagine::never_valid<test_person> p{{"Bill", "Roe"}};
    test.check(not bool(p), "is not true");
    test.check(not p, "is false");
    test.check(not p.has_value(), "has not value");
    test.check_equal(
      p.member(&test_person::given_name).value_or("John"),
      "John",
      "and then member value or");
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
