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
import eagine.core.concepts;
//------------------------------------------------------------------------------
struct inner {
    short s{0};
    bool b{true};
    auto foo() const noexcept -> int {
        return 42;
    }
};

struct outer {
    inner i{};
};
//------------------------------------------------------------------------------
void optional_reference_empty(auto& s) {
    eagitest::case_ test{s, 1, "empty"};
    eagine::optional_reference<int> r{eagine::nothing};

    test.check(eagine::optional_like<decltype(r)>, "optional-like");

    test.check(not r.has_value(), "has not value");
    test.check(not bool(r), "is not true");
    test.check_equal(r.value_or(1234), 1234, "value or 1234");
    test.check_equal(r.value_or(2345), 2345, "value or 2345");

    test.check_equal(
      r.transform([](auto i) { return i + 1; }).value_or(3456),
      3456,
      "transform 3456");

    test.check_equal(
      r.transform([](auto i) { return i + 1; }).value_or(4567),
      4567,
      "transform 4567");

    test.check_equal(
      r.and_then([](auto) -> eagine::optional_reference<long> {
           return {eagine::nothing};
       })
        .value_or(5678L),
      5678L,
      "and then 5678");

    test.check_equal(
      r.or_else([&]() -> int& {
           static int i{678};
           return i;
       })
        .value_or(567),
      678,
      "or else 678");

    r.reset();
    test.check(not r.has_value(), "has not value after reset");
}
//------------------------------------------------------------------------------
void optional_reference_non_empty(auto& s) {
    eagitest::case_ test{s, 2, "non-empty"};

    eagine::optional_reference<outer> r{};

    test.check(eagine::optional_like<decltype(r)>, "optional-like");

    test.check_equal(
      r.member(&outer::i).member(&inner::foo).value_or(2345),
      2345,
      "member call 2345");

    test.check_equal(
      r.member(&outer::i).member(&inner::s).value_or(2345),
      2345,
      "member 2345");

    test.check_equal(
      r.or_else([]() -> auto& {
           static outer so;
           return so;
       })
        .member(&outer::i)
        .member(&inner::foo)
        .value_or(2345),
      42,
      "member call 42");

    test.check_equal(
      r.or_else([]() -> auto& {
           static outer so{{5678, false}};
           return so;
       })
        .member(&outer::i)
        .member(&inner::s)
        .value_or(3456),
      5678,
      "member 5678");

    outer o{};

    r = {o};

    test.ensure(r.has_value(), "has value");
    test.ensure(bool(r), "is true");

    test.check_equal(r.value().i.s, o.i.s, "o.i.s 1");
    o.i.s = 1234;
    test.check_equal(r.value().i.s, o.i.s, "o.i.s 2");

    test.check_equal(
      r.or_else([]() -> auto& {
           static outer so{{6789, false}};
           return so;
       })
        .member(&outer::i)
        .member(&inner::s)
        .value_or(3456),
      o.i.s,
      "member 1234");

    test.check_equal(
      r.transform([](outer& x) -> inner& { return x.i; })
        .transform([](inner& x) -> short& { return x.s; })
        .value_or(2345),
      1234,
      "transform 1234");
    test.check_equal(
      r.and_then(
         [](outer& x) -> eagine::optional_reference<inner> { return {x.i}; })
        .and_then(
          [](inner& x) -> eagine::optional_reference<short> { return {x.s}; })
        .value_or(3456),
      1234,
      "and then 1234");

    test.check_equal(
      r.transform([](auto& x) -> auto& { return x.i; })
        .transform([](auto& x) -> auto& { return x.b; })
        .value_or(true),
      true,
      "transform true");

    o.i.s = 3456;
    test.check_equal(
      r.member(&outer::i).member(&inner::foo).value_or(2345),
      42,
      "member call 42");

    test.check_equal(
      r.member(&outer::i).member(&inner::s).value_or(2345),
      3456,
      "member 3456");

    test.check_equal(
      r.member(&outer::i).member(&inner::s).construct<long>().value_or(1111L),
      3456L,
      "construct 3456");

    r = {eagine::nothing};
    test.check(not r.has_value(), "has not value");
    test.check_equal(
      r.transform([](const outer& x) -> const auto& { return x.i; })
        .transform([](const inner& x) -> const auto& { return x.s; })
        .value_or(2345),
      2345,
      "transform 2345");
    test.check_equal(
      r.and_then(
         [](outer& x) -> eagine::optional_reference<inner> { return {x.i}; })
        .and_then(
          [](inner& x) -> eagine::optional_reference<short> { return {x.s}; })
        .value_or(3456),
      3456,
      "and then 3456");

    o.i.s = 4567;
    test.check_equal(
      r.member(&outer::i).member(&inner::s).value_or(4567),
      4567,
      "transform 4567");

    o.i.s = 5678;
    test.check_equal(
      r.member(&outer::i).member(&inner::s).construct<long>().value_or(1111L),
      1111L,
      "construct 1111");

    r.reset();
    test.check(not r.has_value(), "has not value after reset");
}
//------------------------------------------------------------------------------
void optional_reference_non_empty_const(auto& s) {
    eagitest::case_ test{s, 3, "non-empty const"};

    outer o{};
    eagine::optional_reference<const outer> r{o};

    test.check(eagine::optional_like<decltype(r)>, "optional-like");

    test.ensure(r.has_value(), "has value");
    test.ensure(bool(r), "is true");

    test.check_equal(r.value().i.s, o.i.s, "o.i.s 1");
    o.i.s = 1234;
    test.check_equal(r.value().i.s, o.i.s, "o.i.s 2");

    test.check_equal(
      r.transform([](const outer& x) -> const inner& { return x.i; })
        .transform([](const inner& x) -> const short& { return x.s; })
        .value_or(2345),
      1234,
      "transform 1234");
    test.check_equal(
      r.and_then([](const outer& x) -> eagine::optional_reference<const inner> {
           return {x.i};
       })
        .and_then(
          [](const inner& x) -> eagine::optional_reference<const short> {
              return {x.s};
          })
        .value_or(3456),
      1234,
      "and then 1234");

    test.check_equal(
      r.transform([](const outer& x) -> auto& { return x.i; })
        .transform([](const inner& x) -> auto& { return x.b; })
        .value_or(true),
      true,
      "transform true");

    o.i.s = 3456;
    test.check_equal(
      r.member(&outer::i).member(&inner::s).value_or(2345),
      3456,
      "transform 3456");

    test.check_equal(
      r.member(&outer::i).member(&inner::s).construct<long>().value_or(3333L),
      3456L,
      "construct 3456");

    r = {eagine::nothing};
    test.check(not r.has_value(), "has not value");
    test.check_equal(
      r.transform([](const outer& x) -> const inner& { return x.i; })
        .transform([](const inner& x) -> const short& { return x.s; })
        .value_or(2345),
      2345,
      "transform 2345");
    test.check_equal(
      r.and_then([](const outer& x) -> eagine::optional_reference<const inner> {
           return {x.i};
       })
        .and_then(
          [](const inner& x) -> eagine::optional_reference<const short> {
              return {x.s};
          })
        .value_or(3456),
      3456,
      "and then 3456");

    o.i.s = 4567;
    test.check_equal(
      r.member(&outer::i).member(&inner::s).value_or(4567),
      4567,
      "transform 4567");

    o.i.s = 5678;
    test.check_equal(
      r.member(&outer::i).member(&inner::s).construct<long>().value_or(2222L),
      2222L,
      "construct 2222");

    r.reset();
    test.check(not r.has_value(), "has not value after reset");
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
