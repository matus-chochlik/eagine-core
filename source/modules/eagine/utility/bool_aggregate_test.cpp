/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.utility;
//------------------------------------------------------------------------------
void bool_aggregate_default_construct(auto& s) {
    eagitest::case_ test{s, 1, "default construct"};
    const eagine::some_true t;
    test.constructed(t, "t");
    test.check(not t, "is false");
}
//------------------------------------------------------------------------------
void bool_aggregate_init_construct(auto& s) {
    eagitest::case_ test{s, 2, "construct"};

    const eagine::some_true t{true};
    test.constructed(t, "t");
    test.check(bool(t), "t is true");

    const eagine::some_true f{false};
    test.constructed(f, "f");
    test.check(not f, "f is false");
}
//------------------------------------------------------------------------------
void bool_aggregate_copy_construct(auto& s) {
    eagitest::case_ test{s, 3, "copy construct"};

    const eagine::some_true t0{true};
    const eagine::some_true t{t0};
    test.constructed(t, "t");
    test.check(bool(t), "t is true");

    const eagine::some_true f0{false};
    const eagine::some_true f{f0};
    test.constructed(f, "f");
    test.check(not f, "f is false");
}
//------------------------------------------------------------------------------
void bool_aggregate_call_op_bool(auto& s) {
    eagitest::case_ test{s, 4, "call operator bool"};
    eagine::work_done x;
    test.constructed(x, "x");
    test.check(not x, "is false");
    x(false);
    test.check(not x, "is false");
    x();
    test.check(bool(x), "is true");
    x(false);
    test.check(bool(x), "is true");
}
//------------------------------------------------------------------------------
void bool_aggregate_call_op_self(auto& s) {
    eagitest::case_ test{s, 5, "call operator self"};
    eagine::work_done x;
    test.constructed(x, "x");
    test.check(not x, "is false");

    const eagine::some_true f{false};
    x(f);
    test.check(not x, "is false");

    const eagine::some_true t{true};
    x(t);
    test.check(bool(x), "is true");
    x(f);
    test.check(bool(x), "is true");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "bool_aggregate", 5};
    test.once(bool_aggregate_default_construct);
    test.once(bool_aggregate_init_construct);
    test.once(bool_aggregate_copy_construct);
    test.once(bool_aggregate_call_op_bool);
    test.once(bool_aggregate_call_op_self);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
