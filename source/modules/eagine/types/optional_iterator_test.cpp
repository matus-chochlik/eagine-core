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
void optional_iterator_empty(auto& s) {
    eagitest::case_ test{s, 1, "empty"};

    std::map<int, int> m;
    const auto ri{test.random().get_int(-10000, 10000)};

    const auto empty{eagine::find(m, 123)};
    test.check(eagine::optional_like<decltype(empty)>, "optional-like");

    test.check(not empty.has_value(), "has not value");
    test.check(not empty, "is false");

    test.check_equal(empty.value_or(ri), ri, "value_or");

    test.check_equal(
      empty.and_then([](int i) -> std::optional<int> { return i; }).value_or(ri),
      ri,
      "and then");

    empty.and_then([&](int) { test.fail("should not get here"); });

    test.check_equal(
      empty.transform([](int i) { return i; }).value_or(ri), ri, "transform");
}
//------------------------------------------------------------------------------
void optional_iterator_found(auto& s) {
    eagitest::case_ test{s, 2, "found"};
    eagitest::track trck{test, 0, 1};

    std::map<int, int> m;
    const auto rk{test.random().get_int(-10000, 10000)};
    const auto rv{test.random().get_int(-10000, 10000)};
    const auto ri{test.random().get_int(-10000, 10000)};
    m[rk] = rv;

    const auto found{eagine::find(m, rk)};
    test.check(eagine::optional_like<decltype(found)>, "optional-like");

    test.check(found.has_value(), "has value");
    test.check(bool(found), "is true");

    test.check_equal(found.value_or(ri), rv, "value_or");

    test.check_equal(
      found.and_then([](int i) -> std::optional<int> { return i; }).value_or(ri),
      rv,
      "and then");

    found.and_then([&](int) { trck.checkpoint(1); });

    test.check_equal(
      found.transform([](int i) { return i; }).value_or(ri), rv, "transform");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "optional-iterator", 2};
    test.once(optional_iterator_empty);
    test.once(optional_iterator_found);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>

