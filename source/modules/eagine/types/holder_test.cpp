/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import std;
import eagine.core.types;
//------------------------------------------------------------------------------
// empty
//------------------------------------------------------------------------------
void test_holder_empty(auto& test, auto& h) {
    test.check(not h.has_value(), "has not value");
    test.check(not bool(h), "is not true");
    test.check_equal(h.value_or("ABCDE"), "ABCDE", "value or ABCDE");
    std::string f{"XYZ"};
    test.check_equal(h.value_or(f), "XYZ", "value or XYZ");

    h.and_then([&](auto&) { test.fail("has no value"); });
    test.check_equal(
      h.and_then([&](auto&) -> std::optional<std::string> { return {"PQRS"}; })
        .value_or("GHIJ"),
      "GHIJ",
      "and then GHIJ");
}
//------------------------------------------------------------------------------
void unique_holder_empty(auto& s) {
    eagitest::case_ test{s, 1, "unique empty"};
    eagine::unique_holder<std::string> h{};

    test_holder_empty(test, h);
}
//------------------------------------------------------------------------------
void shared_holder_empty(auto& s) {
    eagitest::case_ test{s, 2, "shared empty"};
    eagine::shared_holder<std::string> h{};

    test_holder_empty(test, h);
}
//------------------------------------------------------------------------------
// string
//------------------------------------------------------------------------------
void test_holder_string(auto& test, auto& h) {
    eagitest::track trck{test, 0, 1};

    test.check(h.has_value(), "has value");
    test.check(bool(h), "is true");

    test.check_equal(h.value_or("ABCDE"), "CDEF", "value or CDEF 1");
    std::string f{"XYZ"};
    test.check_equal(h.value_or(f), "CDEF", "value or CDEF 2");

    h.and_then([&](auto&) { trck.checkpoint(1); });
    test.check(
      h.and_then([&](auto& s) -> eagine::tribool { return s == "CDEF"; })
        .or_false(),
      "and then CDEF");
}
//------------------------------------------------------------------------------
void unique_holder_string(auto& s) {
    eagitest::case_ test{s, 3, "unique string"};
    eagine::unique_holder<std::string> h{eagine::hold<std::string>, "CDEF"};

    test_holder_string(test, h);
}
//------------------------------------------------------------------------------
void shared_holder_string(auto& s) {
    eagitest::case_ test{s, 4, "shared string"};
    eagine::shared_holder<std::string> h{eagine::hold<std::string>, "CDEF"};

    test_holder_string(test, h);
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "holder", 4};
    test.once(unique_holder_empty);
    test.once(shared_holder_empty);
    test.once(unique_holder_string);
    test.once(shared_holder_string);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
