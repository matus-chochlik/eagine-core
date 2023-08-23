/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.types;
import eagine.core.c_api;
//------------------------------------------------------------------------------
struct test_ec_A : eagine::c_api::enum_class<test_ec_A, unsigned, 0U, 0U> {
    using eagine::c_api::enum_class<test_ec_A, unsigned, 0, 0>::enum_class;
};

struct test_ec_B : eagine::c_api::enum_class<test_ec_B, unsigned, 0U, 1U> {
    using eagine::c_api::enum_class<test_ec_B, unsigned, 0, 1>::enum_class;
};

struct test_ec_C : eagine::c_api::enum_class<test_ec_C, unsigned, 0U, 2U> {
    using eagine::c_api::enum_class<test_ec_C, unsigned, 0, 2>::enum_class;
};

struct test_ec_D : eagine::c_api::enum_class<test_ec_D, unsigned, 0U, 3U> {
    using eagine::c_api::enum_class<test_ec_D, unsigned, 0, 3>::enum_class;
};

static constexpr const eagine::c_api::
  enum_value<unsigned, eagine::mp_list<test_ec_A>>
    test_ev_1 = {1U << 0U};
static constexpr const eagine::c_api::
  enum_value<unsigned, eagine::mp_list<test_ec_A, test_ec_B>>
    test_ev_2 = {1U << 1U};
static constexpr const eagine::c_api::
  enum_value<unsigned, eagine::mp_list<test_ec_A, test_ec_B, test_ec_C>>
    test_ev_3 = {1U << 2U};
static constexpr const eagine::c_api::
  enum_value<unsigned, eagine::mp_list<test_ec_B, test_ec_C, test_ec_D>>
    test_ev_4 = {1U << 3U};
static constexpr const eagine::c_api::
  enum_value<unsigned, eagine::mp_list<test_ec_B, test_ec_D>>
    test_ev_5 = {1U << 4U};
//------------------------------------------------------------------------------
void enum_bitfield_1(auto& s) {
    eagitest::case_ test{s, 1, "1"};

    eagine::c_api::enum_bitfield<test_ec_A> a1 = test_ev_1;
    test.check(a1.has(test_ev_1), "a1.has(ev_1)");
    test.check(!a1.has(test_ev_2), "not a1.has(ev_2)");
    test.check(!a1.has(test_ev_3), "not a1.has(ev_3)");

    eagine::c_api::enum_bitfield<test_ec_A> a2 = test_ev_2;
    test.check(!a2.has(test_ev_1), "not a2.has(ev_1)");
    test.check(a2.has(test_ev_2), "a2.has(ev_2)");
    test.check(!a2.has(test_ev_3), "not a2.has(ev_3)");

    eagine::c_api::enum_bitfield<test_ec_A> a13 = test_ev_1 | test_ev_3;
    test.check(a13.has(test_ev_1), "a13.has(ev_1)");
    test.check(!a13.has(test_ev_2), "not a13.has(ev_2)");
    test.check(a13.has(test_ev_3), "a13.has(ev_3)");

    eagine::c_api::enum_bitfield<test_ec_A> a31 = test_ev_3 | test_ev_1;
    test.check(a13 == a31, "a13 == a31");

    eagine::c_api::enum_bitfield<test_ec_A> a123 =
      test_ev_1 | test_ev_2 | test_ev_3;

    test.check(a123.has(test_ev_1), "a123.has(ev_1)");
    test.check(a123.has(test_ev_2), "a123.has(ev_2)");
    test.check(a123.has(test_ev_3), "a123.has(ev_3)");
}
//------------------------------------------------------------------------------
void enum_bitfield_2(auto& s) {
    eagitest::case_ test{s, 2, "2"};

    eagine::c_api::enum_bitfield<test_ec_B> b2 = test_ev_2;
    eagine::c_api::enum_bitfield<test_ec_B> b23 = b2 | test_ev_3;

    test.check(b23.has(test_ev_2), "b23.has(ev_2)");
    test.check(b23.has(test_ev_3), "b23.has(ev_3)");
    test.check(!b23.has(test_ev_4), "not b23.has(ev_4)");
    test.check(!b23.has(test_ev_5), "not b23.has(ev_5)");

    eagine::c_api::enum_bitfield<test_ec_B> b234 = b2 | test_ev_3 | test_ev_4;

    test.check(b234.has(test_ev_2), "b234.has(ev_2)");
    test.check(b234.has(test_ev_3), "b234.has(ev_3)");
    test.check(b234.has(test_ev_4), "b234.has(ev_4)");
    test.check(!b234.has(test_ev_5), "not b234.has(ev_5)");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "C-API enum_bitfield", 2};
    test.once(enum_bitfield_1);
    test.once(enum_bitfield_2);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>

