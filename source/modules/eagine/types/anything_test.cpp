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
void test_anything_default_construct(auto& s) {
    const eagitest::case_ self{"default_construct", s};
    eagine::anything a;
    s.constructed("a", a);
}
//------------------------------------------------------------------------------
void test_anything_construct(auto& s) {
    const eagitest::case_ self{"construct", s};
    eagine::anything ai(0);
    s.constructed("ai", ai);
    eagine::anything af(0.F);
    s.constructed("af", af);
    eagine::anything ad(0.0);
    s.constructed("ad", ad);
}
//------------------------------------------------------------------------------
auto main() -> int {
    eagitest::suite test{"anything"};
    test(test_anything_default_construct);
    test(test_anything_construct);
    return 0;
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
