/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

import eagine.core.types;
#include <eagine/testing/unit_begin.hpp>
//------------------------------------------------------------------------------
void test_anything_default_construct() {
    eagine::anything a;
    (void)a;
}
//------------------------------------------------------------------------------
void test_anything_construct() {
    eagine::anything ai(0);
    (void)ai;
    eagine::anything af(0.F);
    (void)af;
    eagine::anything ad(0.0);
    (void)ad;
}
//------------------------------------------------------------------------------
auto main() -> int {
    test_anything_default_construct();
    return 0;
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
