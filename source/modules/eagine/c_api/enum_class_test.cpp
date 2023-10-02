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
    test_ev_1 = {1};
static constexpr const eagine::c_api::
  enum_value<unsigned, eagine::mp_list<test_ec_A, test_ec_B>>
    test_ev_2 = {2};
static constexpr const eagine::c_api::
  enum_value<unsigned, eagine::mp_list<test_ec_A, test_ec_B, test_ec_C>>
    test_ev_3 = {3};
static constexpr const eagine::c_api::
  enum_value<unsigned, eagine::mp_list<test_ec_B, test_ec_C, test_ec_D>>
    test_ev_4 = {4};
static constexpr const eagine::c_api::
  enum_value<unsigned, eagine::mp_list<test_ec_B, test_ec_D>>
    test_ev_5 = {5};
static constexpr const eagine::c_api::
  enum_value<unsigned, eagine::mp_list<test_ec_D>>
    test_ev_6 = {6};
//------------------------------------------------------------------------------
void enum_class_1(auto& s) {
    eagitest::case_ test{s, 1, "1"};

    test_ec_A a = test_ev_1;
    test.check(a == test_ev_1, "a == ev_1");
    test.check(a != test_ev_2, "a != ev_2");
    test.check(a != test_ev_3, "a != ev_3");
    a = test_ev_2;
    test.check(a != test_ev_1, "a != ev_1");
    test.check(a == test_ev_2, "a == ev_2");
    test.check(a != test_ev_3, "a != ev_3");
    a = test_ev_3;
    test.check(a != test_ev_1, "a != ev_1");
    test.check(a != test_ev_2, "a != ev_2");
    test.check(a == test_ev_3, "a == ev_3");

    test_ec_B b = test_ev_2;
    test.check(b == test_ev_2, "b != ev_2");
    test.check(b != test_ev_3, "b != ev_3");
    test.check(b != test_ev_4, "b != ev_4");
    test.check(b != test_ev_5, "b != ev_5");
    b = test_ev_3;
    test.check(b != test_ev_2, "b != ev_2");
    test.check(b == test_ev_3, "b != ev_3");
    test.check(b != test_ev_4, "b != ev_4");
    test.check(b != test_ev_5, "b != ev_5");
    b = test_ev_4;
    test.check(b != test_ev_2, "b != ev_2");
    test.check(b != test_ev_3, "b != ev_3");
    test.check(b == test_ev_4, "b != ev_4");
    test.check(b != test_ev_5, "b != ev_5");
    b = test_ev_5;
    test.check(b != test_ev_2, "b != ev_2");
    test.check(b != test_ev_3, "b != ev_3");
    test.check(b != test_ev_4, "b != ev_4");
    test.check(b == test_ev_5, "b != ev_5");

    test_ec_C c = test_ev_3;
    test.check(c == test_ev_3, "c != ev_3");
    test.check(c != test_ev_4, "c != ev_4");
    c = test_ev_4;
    test.check(c != test_ev_3, "c != ev_3");
    test.check(c == test_ev_4, "c != ev_4");

    test_ec_D d = test_ev_5;
    test.check(d == test_ev_5, "d != ev_5");
    test.check(d != test_ev_6, "d != ev_6");
    d = test_ev_6;
    test.check(d != test_ev_5, "d != ev_5");
    test.check(d == test_ev_6, "d != ev_6");
}
//------------------------------------------------------------------------------
void enum_class_2(auto& s) {
    eagitest::case_ test{s, 2, "2"};

    test_ec_A a = test_ev_1;
    test.check(unsigned(a) == unsigned(test_ev_1), "a == ev_1");
    test.check(unsigned(a) != unsigned(test_ev_2), "a != ev_2");
    test.check(unsigned(a) != unsigned(test_ev_3), "a != ev_3");

    a = test_ev_2;
    test.check(unsigned(a) != unsigned(test_ev_1), "a != ev_1");
    test.check(unsigned(a) == unsigned(test_ev_2), "a == ev_2");
    test.check(unsigned(a) != unsigned(test_ev_3), "a != ev_3");

    a = test_ev_3;
    test.check(unsigned(a) != unsigned(test_ev_1), "a != ev_1");
    test.check(unsigned(a) != unsigned(test_ev_2), "a != ev_2");
    test.check(unsigned(a) == unsigned(test_ev_3), "a == ev_3");
}
//------------------------------------------------------------------------------
void enum_class_3(auto& s) {
    eagitest::case_ test{s, 3, "3"};

    test_ec_A a = test_ev_1;
    test_ec_B b = test_ev_2;

    eagine::c_api::any_enum_value<0U> aev = a;
    aev = b;
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "C-API enum_class", 3};
    test.once(enum_class_1);
    test.once(enum_class_2);
    test.once(enum_class_3);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
