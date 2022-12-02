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
struct type_A {};
struct type_B {};
struct type_C {};
struct type_D {};
//------------------------------------------------------------------------------
void mp_list_contains(auto& s) {
    eagitest::case_ test{s, 1, "contains"};

    using namespace eagine;
    using lABC = mp_list<type_A, type_B, type_C>;
    using lDCB = mp_list<type_D, type_C, type_B>;

    test.check(mp_contains_v<lABC, type_A>, "ABC contains A");
    test.check(mp_contains_v<lABC, type_B>, "ABC contains B");
    test.check(mp_contains_v<lABC, type_C>, "ABC contains C");
    test.check(!mp_contains_v<lABC, type_D>, "ABC does not contain D");

    test.check(mp_contains_v<lDCB, type_D>, "DCB contains D");
    test.check(mp_contains_v<lDCB, type_C>, "DCB contains C");
    test.check(mp_contains_v<lDCB, type_B>, "DCB contains B");
    test.check(!mp_contains_v<lDCB, type_A>, "DCB does not contain A");
}
//------------------------------------------------------------------------------
void mp_list_union(auto& s) {
    eagitest::case_ test{s, 2, "union"};

    using namespace eagine;
    using lABC = mp_list<type_A, type_B, type_C>;
    using lDCB = mp_list<type_D, type_C, type_B>;

    using lBC = mp_union_t<lABC, lDCB>;

    test.check(!mp_contains_v<lBC, type_A>, "does not contain A");
    test.check(mp_contains_v<lBC, type_B>, "contains B");
    test.check(mp_contains_v<lBC, type_C>, "contains C");
    test.check(!mp_contains_v<lBC, type_D>, "does not contain D");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "metaprogramming", 2};
    test.once(mp_list_contains);
    test.once(mp_list_union);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
