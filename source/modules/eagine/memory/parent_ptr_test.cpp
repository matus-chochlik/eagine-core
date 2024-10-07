/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import std;
import eagine.core.memory;
//------------------------------------------------------------------------------
struct test_outer;

struct test_inner {
    eagine::memory::basic_parent_ptr<test_outer, std::int16_t> parent;

    test_inner(test_outer& p) noexcept
      : parent{p} {}
};

struct test_outer {
    test_inner inner1{*this};
    test_inner inner2{*this};
};
//------------------------------------------------------------------------------
void parent_ptr_default_construct(auto& s) {
    eagitest::case_ test{s, 1, "default construct"};
    using namespace eagine;

    test_outer outer;
    std::cout << outer.inner1.parent.offset() << std::endl;
    std::cout << outer.inner2.parent.offset() << std::endl;

    test.check(outer.inner1.parent.get() == &outer, "get 1");
    test.check(outer.inner2.parent.get() == &outer, "get 2");

    test.check(
      static_cast<test_outer*>(outer.inner1.parent) == &outer, "cast 1");
    test.check(
      static_cast<test_outer*>(outer.inner2.parent) == &outer, "cast 2");
}
//------------------------------------------------------------------------------
void parent_ptr_copy_construct_1(auto& s) {
    eagitest::case_ test{s, 2, "copy construct"};
    using namespace eagine;

    test_outer orig;
    test_outer copy{orig};

    test.check(copy.inner1.parent.get() == &copy, "get 1");
    test.check(copy.inner2.parent.get() == &copy, "get 2");

    test.check(copy.inner1.parent.get() != &orig, "not get 1");
    test.check(copy.inner2.parent.get() != &orig, "not get 2");

    test.check(static_cast<test_outer*>(copy.inner1.parent) == &copy, "cast 1");
    test.check(static_cast<test_outer*>(copy.inner2.parent) == &copy, "cast 2");

    test.check(
      static_cast<test_outer*>(copy.inner1.parent) != &orig, "not cast 1");
    test.check(
      static_cast<test_outer*>(copy.inner2.parent) != &orig, "not cast 2");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "parent_ptr", 2};
    test.once(parent_ptr_default_construct);
    test.once(parent_ptr_copy_construct_1);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
