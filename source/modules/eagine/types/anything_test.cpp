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
void anything_default_construct(auto& s) {
    eagitest::case_ test{s, "default construct"};
    eagine::anything a;
    test.constructed(a, "a");
}
//------------------------------------------------------------------------------
void anything_construct(auto& s) {
    eagitest::case_ test{s, "construct"};
    eagine::anything ai(0);
    test.constructed(ai, "ai");
    eagine::anything af(0.F);
    test.constructed(af, "af");
    eagine::anything ad(0.0);
    test.constructed(ad, "ad");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "anything"};
    test.once(anything_default_construct);
    test.once(anything_construct);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
