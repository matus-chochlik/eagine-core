/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.types;
//------------------------------------------------------------------------------
void nothing_default_construct(auto& s) {
    eagitest::case_ test{s, 1, "default construct"};
    eagine::nothing_t n;
    test.constructed(n, "n");
}
//------------------------------------------------------------------------------
void nothing_constant(auto& s) {
    eagitest::case_ test{s, 2, "constant"};
    eagine::nothing_t n{eagine::nothing};
    test.constructed(n, "n");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "nothing", 2};
    test.once(nothing_default_construct);
    test.once(nothing_constant);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
