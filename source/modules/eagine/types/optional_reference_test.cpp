/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.types;
//------------------------------------------------------------------------------
struct inner {
    short s{0};
    bool b{true};
};

struct outer {
    inner i{};
};
//------------------------------------------------------------------------------
void optional_reference_empty(auto& s) {
    eagitest::case_ test{s, 1, "empty"};
    eagine::optional_reference<int> r{eagine::nothing};

    test.check(not r.has_value(), "has not value");
    test.check(not bool(r), "is not true");
    test.check_equal(r.value_or(1234), 1234, "value or 1234");
    test.check_equal(r.value_or(2345), 2345, "value or 2345");

    test.check_equal(
      r.and_then([](auto i) { return i + 1; }).value_or(3456),
      3456,
      "and then 3456");

    test.check_equal(
      r.and_then([](auto i) { return i + 1; }).value_or(4567),
      4567,
      "and then 4567");
}
//------------------------------------------------------------------------------
void optional_reference_non_empty(auto& s) {
    eagitest::case_ test{s, 2, "non-empty"};

    outer o{};
    eagine::optional_reference<outer> r{o};

    test.ensure(r.has_value(), "has value");
    test.ensure(bool(r), "is true");

    test.check_equal(r.value().i.s, o.i.s, "o.i.s 1");
    o.i.s = 1234;
    test.check_equal(r.value().i.s, o.i.s, "o.i.s 2");

    test.check_equal(
      r.and_then([](outer& x) -> inner& { return x.i; })
        .and_then([](inner& x) -> short& { return x.s; })
        .value_or(2345),
      1234,
      "and then 1234");
    test.check_equal(
      r.and_then([](auto& x) -> auto& { return x.i; })
        .and_then([](auto& x) -> auto& { return x.b; })
        .value_or(true),
      true,
      "and then true");

    o.i.s = 3456;
    test.check_equal(
      r.member(&outer::i).member(&inner::s).value_or(2345),
      3456,
      "and then 3456");

    r = {eagine::nothing};
    test.check(not r.has_value(), "has not value");
    test.check_equal(
      r.and_then([](const outer& x) -> const auto& { return x.i; })
        .and_then([](const inner& x) -> const auto& { return x.s; })
        .value_or(2345),
      2345,
      "and then 2345");

    o.i.s = 4567;
    test.check_equal(
      r.member(&outer::i).member(&inner::s).value_or(4567),
      4567,
      "and then 4567");
}
//------------------------------------------------------------------------------
void optional_reference_non_empty_const(auto& s) {
    eagitest::case_ test{s, 3, "non-empty const"};

    outer o{};
    eagine::optional_reference<const outer> r{o};

    test.ensure(r.has_value(), "has value");
    test.ensure(bool(r), "is true");

    test.check_equal(r.value().i.s, o.i.s, "o.i.s 1");
    o.i.s = 1234;
    test.check_equal(r.value().i.s, o.i.s, "o.i.s 2");

    test.check_equal(
      r.and_then([](const outer& x) -> const inner& { return x.i; })
        .and_then([](const inner& x) -> const short& { return x.s; })
        .value_or(2345),
      1234,
      "and then 1234");
    test.check_equal(
      r.and_then([](const outer& x) -> auto& { return x.i; })
        .and_then([](const inner& x) -> auto& { return x.b; })
        .value_or(true),
      true,
      "and then true");

    o.i.s = 3456;
    test.check_equal(
      r.member(&outer::i).member(&inner::s).value_or(2345),
      3456,
      "and then 3456");

    r = {eagine::nothing};
    test.check(not r.has_value(), "has not value");
    test.check_equal(
      r.and_then([](const outer& x) -> const inner& { return x.i; })
        .and_then([](const inner& x) -> const short& { return x.s; })
        .value_or(2345),
      2345,
      "and then 2345");

    o.i.s = 4567;
    test.check_equal(
      r.member(&outer::i).member(&inner::s).value_or(4567),
      4567,
      "and then 4567");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "optional reference", 3};
    test.once(optional_reference_empty);
    test.once(optional_reference_non_empty);
    test.once(optional_reference_non_empty_const);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
