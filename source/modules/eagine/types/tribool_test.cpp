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
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "tribool", 2};
    test.once(tribool_default_construct);
    test.once(tribool_ops);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
