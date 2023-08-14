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
static_assert(
  eagine::optional_like<eagine::optional_like_tuple<
    std::optional<std::string>,
    eagine::optionally_valid<std::string>,
    eagine::optional_reference<std::string>>>,
  "optional-like");
//------------------------------------------------------------------------------
void optional_like_tuple_empty(auto& s) {
    eagitest::case_ test{s, 1, "empty"};

    eagine::optional_like_tuple<
      std::optional<std::string>,
      eagine::optionally_valid<std::string>,
      eagine::optional_reference<std::string>>
      opts;

    test.check(eagine::optional_like<decltype(opts)>, "optional-like");

    test.check(not opts.has_value(), "has not value");
    test.check(not opts, "is false");

    const auto [v1, v2, v3] = opts.value_or("foo", "bar", "baz");
    test.check_equal(v1, "foo", "v1 ok");
    test.check_equal(v2, "bar", "v2 ok");
    test.check_equal(v3, "baz", "v3 ok");

    std::string foo{"foo"};
    std::string bar{"bar"};
    std::string baz{"baz"};

    const auto& [r1, r2, r3] = opts.value_or(foo, bar, baz);

    foo = "fooz";
    bar = "barz";
    baz = "buzz";

    test.check_equal(r1, "foo", "r1 ok");
    test.check_equal(r2, "barz", "r2 ok");
    test.check_equal(r3, "buzz", "r3 ok");

    const auto rs{test.random().get_std_size(1, 10000)};
    test.check_equal(
      opts
        .and_then(
          [](std::string_view p1, std::string_view p2, std::string_view p3)
            -> std::optional<std::size_t> {
              return p1.size() + p2.size() + p3.size();
          })
        .value_or(rs),
      rs,
      "and then 1");

    opts.and_then(
      [&](const std::string&, const std::string&, const std::string&) {
          test.fail("should not get here");
      });

    test.check_equal(
      opts
        .transform([](const auto& p1, const auto& p2, const auto& p3) {
            return p1.size() + p2.size() + p3.size();
        })
        .value_or(rs + 1),
      rs + 1,
      "transform size");

    test.check(
      opts
        .transform([](const auto& p1, const auto& p2, const auto& p3) {
            return p1.empty() and p2.empty() and p3.empty();
        })
        .or_true(),
      "transform empty");
}
//------------------------------------------------------------------------------
void optional_like_tuple_meld_1(auto& s) {
    eagitest::case_ test{s, 2, "meld 1"};
    eagitest::track trck{test, 1, 1};

    const int foo{s.random().get_int(-10000, 10000)};
    const int bar{s.random().get_int(-10000, 10000)};
    const int baz{s.random().get_int(-10000, 10000)};

    int i = baz;

    const auto opti{eagine::meld(
      std::optional<int>{foo},
      eagine::optionally_valid<int>{bar, true},
      eagine::optional_reference<int>{i})};

    test.check(eagine::optional_like<decltype(opti)>, "optional-like");

    test.check(opti.has_value(), "has value");
    test.check(bool(opti), "is true");

    const auto [v1, v2, v3] = opti.value();
    test.check_equal(v1, foo, "v1 ok");
    test.check_equal(v2, bar, "v2 ok");
    test.check_equal(v3, baz, "v3 ok");

    const auto [vo1, vo2, vo3] = opti.value_or(234, 345, 456);
    test.check_equal(vo1, foo, "vo1 ok");
    test.check_equal(vo2, bar, "vo2 ok");
    test.check_equal(vo3, baz, "vo3 ok");

    test.check_equal(
      opti
        .and_then([](int p1, int p2, int p3) -> std::optional<int> {
            return p1 + p2 + p3;
        })
        .value_or(0),
      foo + bar + baz,
      "and then 1");

    opti.and_then([&](int, int, int) { trck.checkpoint(1); });

    test.check_equal(
      opti.transform([](int p1, int p2, int p3) { return p1 + p2 + p3; })
        .value_or(0),
      foo + bar + baz,
      "transform add");

    test.check_equal(
      opti.transform([](int p1, int p2, int p3) { return p1 - p2 - p3; })
        .value_or(0),
      foo - bar - baz,
      "transform sub");

    test.check_equal(
      opti.transform([](int p1, int p2, int p3) { return p1 + p2 < p3; })
        .value_or(foo + bar >= baz),
      foo + bar < baz,
      "transform cmp");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "optional-like tuple", 2};
    test.once(optional_like_tuple_empty);
    test.once(optional_like_tuple_meld_1);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>

