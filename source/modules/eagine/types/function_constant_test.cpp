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
struct bar {
    int i;

    int foo(int j) const {
        return i + j;
    }

    float foo(int j, float k) {
        return float(i + j) / k;
    }
};

struct baz {
    int i;

    baz& inc() {
        ++i;
        return *this;
    }
};
//------------------------------------------------------------------------------
void mem_func_const_pointer_get(eagitest::suite& s) {
    eagitest::case_ test{s, 1, "member_get"};

    using fubar1 =
      eagine::member_function_constant<int (bar::*)(int) const, &bar::foo>;
    using fubar2 =
      eagine::member_function_constant<float (bar::*)(int, float), &bar::foo>;
    using incbaz = eagine::member_function_constant<baz& (baz::*)(), &baz::inc>;

    fubar1::pointer pfubar1 = fubar1::get();
    fubar2::pointer pfubar2 = fubar2::get();
    incbaz::pointer pincbaz = incbaz::get();

    bar br{432};

    test.check_equal(((br).*(pfubar1))(543), 432 + 543, "A");
    test.check_equal(((br).*(pfubar2))(543, 654.3f), (432 + 543) / 654.3f, "B");

    baz bz{1};

    test.check_equal(bz.i, 1, "C");
    ((bz).*(pincbaz))();
    test.check_equal(bz.i, 2, "D");
}
//------------------------------------------------------------------------------
void mem_func_const_scope(eagitest::suite& s) {
    eagitest::case_ test{s, 2, "member_scope"};

    using fubar1 =
      eagine::member_function_constant<int (bar::*)(int) const, &bar::foo>;
    using fubar2 =
      eagine::member_function_constant<float (bar::*)(int, float), &bar::foo>;
    using incbaz = eagine::member_function_constant<baz& (baz::*)(), &baz::inc>;

    test.check(std::is_same<bar, fubar1::scope>::value, "A");
    test.check(std::is_same<bar, fubar2::scope>::value, "B");
    test.check(not std::is_same<baz, fubar1::scope>::value, "C");
    test.check(not std::is_same<baz, fubar2::scope>::value, "D");
    test.check(not std::is_same<bar, incbaz::scope>::value, "E");
    test.check(std::is_same<baz, incbaz::scope>::value, "F");
}
//------------------------------------------------------------------------------
void mem_func_const_make_free(eagitest::suite& s) {
    eagitest::case_ test{s, 3, "member_free"};

    using fubar1 =
      eagine::member_function_constant<int (bar::*)(int) const, &bar::foo>;
    using fubar2 =
      eagine::member_function_constant<float (bar::*)(int, float), &bar::foo>;
    using incbaz = eagine::member_function_constant<baz& (baz::*)(), &baz::inc>;

    bar br{123};

    test.check_equal(fubar1::make_free()(&br, 234), 123 + 234, "A");
    test.check_equal(
      fubar2::make_free()(&br, 234, 345.6f), (123 + 234) / 345.6f, "A");

    baz bz{1};
    test.check_equal(bz.i, 1, "A");
    incbaz::make_free()(&bz);
    test.check_equal(bz.i, 2, "A");
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "function constant", 3};
    test.once(mem_func_const_pointer_get);
    test.once(mem_func_const_scope);
    test.once(mem_func_const_make_free);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
