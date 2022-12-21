/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.types;
import eagine.core.utility;
//------------------------------------------------------------------------------
float foo(float a, float b, float c) {
    return (a - b) / (a + c);
}

struct bar {
    int i;

    auto foo(int j) const noexcept -> int {
        return i + j;
    }

    auto foo(float j, float k, float l) const -> float {
        return (float(i) + j) / (k - l);
    }

    static auto foo(bar* b, float j, float k, float l) noexcept -> float {
        return b->foo(j, k, l);
    }

    auto operator()(float a, float b, float c) -> float {
        return foo(a, b, c);
    }
};

struct baz {
    int i;

    auto inc() -> baz& {
        ++i;
        return *this;
    }

    auto add(int c) noexcept -> baz& {
        i += c;
        return *this;
    }
};
//------------------------------------------------------------------------------
void callable_ref_default_construct(auto& s) {
    eagitest::case_ test{s, 1, "default construct"};

    eagine::callable_ref<float(float, float, float)> c;
    test.constructed(c, "c");

    test.check_equal(bool(c), false, "is false");
    test.check_equal(not(c), true, "is not true");
}
//------------------------------------------------------------------------------
void callable_ref_foo(auto& s) {
    eagitest::case_ test{s, 2, "foo"};

    eagine::callable_ref<float(float, float, float)> c(&::foo);

    test.check_equal(bool(c), true, "is true");
    test.check_equal(not(c), false, "is not false");

    test.check_equal(
      c(234.5f, 345.6f, 456.7f),
      foo(234.5f, 345.6f, 456.7f),
      "same call result");
}
//------------------------------------------------------------------------------
void callable_ref_bar_foo(auto& s) {
    eagitest::case_ test{s, 3, "bar::foo"};

    bar br{123};

    eagine::callable_ref<float(float, float, float) noexcept> c(&br, &bar::foo);

    test.check_equal(bool(c), true, "is true");
    test.check_equal(not(c), false, "is not false");

    test.check_equal(
      c(234.5f, 345.6f, 456.7f),
      bar::foo(&br, 234.5f, 345.6f, 456.7f),
      "same call result");
}
//------------------------------------------------------------------------------
void callable_ref_bar_call_op(auto& s) {
    eagitest::case_ test{s, 4, "bar::operator()"};

    bar br{234};

    eagine::callable_ref<float(float, float, float)> c(
      eagine::construct_from, br);

    test.check_equal(bool(c), true, "is true");
    test.check_equal(not(c), false, "is not false");

    test.check_equal(
      c(456.7f, 567.8f, 678.9f),
      br(456.7f, 567.8f, 678.9f),
      "same call result");
}
//------------------------------------------------------------------------------
void callable_ref_bar_foo_2(auto& s) {
    eagitest::case_ test{s, 5, "bar::foo"};

    bar br{234};

    eagine::callable_ref<float(float, float, float)> c(
      &br,
      eagine::member_function_constant<
        float (bar::*)(float, float, float) const,
        &bar::foo>());

    test.check_equal(bool(c), true, "is true");
    test.check_equal(not(c), false, "is not false");

    test.check_equal(
      c(567.8f, 678.9f, 789.0f),
      br.foo(567.8f, 678.9f, 789.0f),
      "same call result");
}
//------------------------------------------------------------------------------
void callable_ref_baz_inc(auto& s) {
    eagitest::case_ test{s, 6, "baz::inc"};

    baz bz{2};
    eagine::callable_ref<baz&()> c(
      &bz, eagine::member_function_constant<baz& (baz::*)(), &baz::inc>());

    test.check_equal(bool(c), true, "is true");
    test.check_equal(not(c), false, "is not false");

    test.check_equal(bz.i, 2, "2");
    c();
    test.check_equal(bz.i, 3, "3");
    c();
    test.check_equal(bz.i, 4, "4");
}
//------------------------------------------------------------------------------
void callable_ref_baz_add(auto& s) {
    eagitest::case_ test{s, 7, "baz::add"};

    baz bz{3};

    eagine::callable_ref<baz&(int) noexcept> c(
      &bz,
      eagine::member_function_constant<baz& (baz::*)(int) noexcept, &baz::add>());

    test.check_equal(bool(c), true, "is true");
    test.check_equal(not(c), false, "is not false");

    test.check_equal(bz.i, 3, "3");
    c(3);
    test.check_equal(bz.i, 6, "6");
    c(2);
    test.check_equal(bz.i, 8, "8");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "callable_ref", 7};
    test.once(callable_ref_default_construct);
    test.once(callable_ref_foo);
    test.once(callable_ref_bar_foo);
    test.once(callable_ref_bar_call_op);
    test.once(callable_ref_bar_foo_2);
    test.once(callable_ref_baz_inc);
    test.once(callable_ref_baz_add);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
