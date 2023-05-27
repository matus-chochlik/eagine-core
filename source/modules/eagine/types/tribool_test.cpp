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
void tribool_default_construct(auto& s) {
    eagitest::case_ test{s, 1, "default construct"};
    eagine::tribool x;
    test.constructed(x, "x");
}
//------------------------------------------------------------------------------
#ifdef CHECK
#undef CHECK
#endif
#define CHECK(X) check(bool(X), #X)
//------------------------------------------------------------------------------
void tribool_ops(auto& s) {
    using eagine::indeterminate;
    eagitest::case_ test{s, 2, "ops"};

    eagine::tribool a = true;
    eagine::tribool b = false;
    eagine::tribool c = indeterminate;

    test.check(eagine::optional_like<decltype(a)>, "optional-like a");
    test.check(eagine::optional_like<decltype(b)>, "optional-like b");
    test.check(eagine::optional_like<decltype(c)>, "optional-like c");

    test.CHECK(bool(a));
    test.CHECK(!!a);
    test.CHECK(not a.is(indeterminate));
    test.CHECK(~a);
    test.CHECK(!!~a);
    test.CHECK(a == a);
    test.CHECK(a == true);
    test.CHECK(!bool(b));
    test.CHECK(!b);
    test.CHECK(not b.is(indeterminate));
    test.CHECK(!bool(~b));
    test.CHECK(!~b);
    test.CHECK(b == b);
    test.CHECK(b == false);
    test.CHECK(!(a == b));
    test.CHECK(!(b == a));
    test.CHECK(!bool(c));
    test.CHECK(!!c);
    test.CHECK(not c.has_value());
    test.CHECK(c.is(indeterminate));
    test.CHECK(bool(~c));
    test.CHECK(!~c);

    test.CHECK(a != false);
    test.CHECK(b != true);
    test.CHECK(a.value());
    test.CHECK(not b.value());
    test.CHECK(!(b != false));
    test.CHECK(!(a != a));
    test.CHECK(!(b != b));
    test.CHECK(a != b);
    test.CHECK(b != a);

    test.CHECK(!!(a == c));
    test.CHECK(!!(b == c));
    test.CHECK(!!(c == c));
    test.CHECK(!!(c == a));
    test.CHECK(!!(c == b));

    test.CHECK(bool(~(a == c)));
    test.CHECK(bool(~(b == c)));
    test.CHECK(bool(~(c == c)));
    test.CHECK(bool(~(c == a)));
    test.CHECK(bool(~(c == b)));

    test.CHECK(bool(~(a != c)));
    test.CHECK(bool(~(b != c)));
    test.CHECK(bool(~(c != c)));
    test.CHECK(bool(~(c != a)));
    test.CHECK(bool(~(c != b)));

    test.CHECK(a && a);
    test.CHECK(!(a && b));
    test.CHECK(!(b && a));
    test.CHECK(!(b && b));
    test.CHECK((a && c).is(indeterminate));
    test.CHECK(!(b && c));
    test.CHECK((c && c).is(indeterminate));
    test.CHECK((c && a).is(indeterminate));
    test.CHECK(!(c && b));

    test.CHECK(a || a);
    test.CHECK(a || b);
    test.CHECK(a || c);
    test.CHECK(b || a);
    test.CHECK(c || a);
    test.CHECK(!(b || b));
    test.CHECK(not(c || b).has_value());
    test.CHECK((c || b).is(indeterminate));
    test.CHECK((b || c).is(indeterminate));
    test.CHECK((c || c).is(indeterminate));
}
//------------------------------------------------------------------------------
void tribool_monadic(auto& s) {
    using eagine::indeterminate;
    using eagine::tribool;
    eagitest::case_ test{s, 3, "monadic"};
    eagitest::track trck{test, 0, 4};

    tribool a = true;
    tribool b = false;
    tribool c = indeterminate;

    test.check(a.value_or(false), "a value or");
    test.check(not b.value_or(true), "b value or");
    test.check(not c.value_or(false), "c value or false");
    test.check(c.value_or(true), "c value or true");

    test.check(
      a.and_then([&](bool v) {
           trck.checkpoint(1);
           return tribool{v};
       })
        .value_or(false),
      "and then value a");

    test.check(
      not b.and_then([&](bool v) {
               trck.checkpoint(2);
               return tribool{v};
           })
            .value_or(true),
      "and then value b");

    test.check(
      c.and_then([&](bool) -> tribool { return false; }).value_or(true),
      "and then c true");

    test.check(
      not c.and_then([&](bool) -> tribool { return true; }).value_or(false),
      "and then c false");

    test.check(
      a.or_else([] { return tribool{false}; }).value(), "or else value a");
    test.check(
      not b.or_else([] { return tribool{true}; }).value(), "or else value b");
    test.check(
      c.or_else([&] {
           trck.checkpoint(3);
           return true;
       })
        .value(),
      "or else c true");
    test.check(
      not c.or_else([&] {
               trck.checkpoint(4);
               return false;
           })
            .value(),
      "or else c false");

    a.reset();
    test.check(not a.has_value(), "a has not value after reset");
    b.reset();
    test.check(not b.has_value(), "b has not value after reset");
    c.reset();
    test.check(not c.has_value(), "c has not value after reset");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "tribool", 3};
    test.once(tribool_default_construct);
    test.once(tribool_ops);
    test.once(tribool_monadic);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
