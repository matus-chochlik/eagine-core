/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.types;
import <array>;
import <tuple>;
//
import <iostream>;
import <iomanip>;
//------------------------------------------------------------------------------
enum class test_bit : unsigned {
    bit_0 = 1U << 0U,
    bit_1 = 1U << 1U,
    bit_2 = 1U << 2U,
    bit_3 = 1U << 3U,
    bit_4 = 1U << 4U,
    bit_5 = 1U << 5U,
    bit_6 = 1U << 6U,
    bit_7 = 1U << 7U
};

auto test_bit_list() noexcept -> std::array<std::tuple<unsigned, test_bit>, 8> {
    return {
      {{0U, test_bit::bit_0},
       {1U, test_bit::bit_1},
       {2U, test_bit::bit_2},
       {3U, test_bit::bit_3},
       {4U, test_bit::bit_4},
       {5U, test_bit::bit_5},
       {6U, test_bit::bit_6},
       {7U, test_bit::bit_7}}};
}
//------------------------------------------------------------------------------
// default construction
//------------------------------------------------------------------------------
void bitfield_default_construct(auto& s) {
    eagitest::case_ test{s, 1, "default construct"};
    eagitest::track trck{test, 0, 4};

    eagine::bitfield<test_bit> b;
    test.constructed(b, "b");
    test.check(b.is_empty(), "is empty");
    test.check(!b, "is false");
    //
    for(auto [idx, bit] : test_bit_list()) {
        test.check(!b.has(bit), "!has(bit_{})", idx);
        trck.passed_part(1);
    }
    //
    for(auto [idx, bit] : test_bit_list()) {
        test.check(b.has_not(bit), "has_not(bit_{}) bit", idx);
        trck.passed_part(2);
    }
    //
    for(auto [idx, bit] : test_bit_list()) {
        test.check(!b.has_only(bit), "!has_only(bit_{})", idx);
        trck.passed_part(3);
    }
    //
    for(auto [idx, bit] : test_bit_list()) {
        test.check(b.has_at_most(bit), "has_at_most(bit_{})", idx);
        trck.passed_part(4);
    }
    //
    test.check(
      b.has_none(
        test_bit::bit_0,
        test_bit::bit_1,
        test_bit::bit_2,
        test_bit::bit_3,
        test_bit::bit_4,
        test_bit::bit_5,
        test_bit::bit_6,
        test_bit::bit_7),
      "has_none");
    //
    test.check(
      !b.has_any(
        test_bit::bit_0,
        test_bit::bit_1,
        test_bit::bit_2,
        test_bit::bit_3,
        test_bit::bit_4,
        test_bit::bit_5,
        test_bit::bit_6,
        test_bit::bit_7),
      "!has_any");
    //
    test.check(
      !b.has_all(
        test_bit::bit_0,
        test_bit::bit_1,
        test_bit::bit_2,
        test_bit::bit_3,
        test_bit::bit_4,
        test_bit::bit_5,
        test_bit::bit_6,
        test_bit::bit_7),
      "!has_all");
}
//------------------------------------------------------------------------------
// underlying bits
//------------------------------------------------------------------------------
void bitfield_underlying_bits_construct_impl(unsigned u, auto& s) {
    eagitest::case_ test{s, 2, "underlying construct"};
    eagitest::track trck{test, 0, 3};

    test.parameter(u, "underlying");

    eagine::bitfield<test_bit> b{u};
    test.constructed(b, "b");
    test.check(b.is_empty() == (u == 0U), "empty if no bits set");
    test.check(!b == (u == 0U), "false if no bits set");
    test.check(bool(b) == (u != 0U), "true if bits set");
    //
    for(auto [idx, bit] : test_bit_list()) {
        test.check(
          b.has(bit) == (((u >> idx) & 0x1U) == 0x1U), "has(bit_{})", idx);
        trck.passed_part(1);
    }
    //
    for(auto [idx, bit] : test_bit_list()) {
        test.check(
          b.has_not(bit) == (((u >> idx) & 0x1U) != 0x1U),
          "has_not(bit_{}) bit",
          idx);
        trck.passed_part(2);
    }
    //
    for(auto [idx, bit] : test_bit_list()) {
        test.check(
          b.has_only(bit) ==
            ((((~(1U << idx) & 0xFFU) & u) == 0x00U) && (u != 0U)),
          "has_only(bit_{})",
          idx);
        trck.passed_part(3);
    }
    //
}
//------------------------------------------------------------------------------
void bitfield_underlying_bits_construct(unsigned run, auto& s) {
    if(run < 1) {
        bitfield_underlying_bits_construct_impl(0x00U, s);
        bitfield_underlying_bits_construct_impl(0x01U, s);
        bitfield_underlying_bits_construct_impl(0x02U, s);
        bitfield_underlying_bits_construct_impl(0x04U, s);
        bitfield_underlying_bits_construct_impl(0x08U, s);
        bitfield_underlying_bits_construct_impl(0x10U, s);
        bitfield_underlying_bits_construct_impl(0x20U, s);
        bitfield_underlying_bits_construct_impl(0x40U, s);
        bitfield_underlying_bits_construct_impl(0x80U, s);
        bitfield_underlying_bits_construct_impl(0xFFU, s);
    } else {
        bitfield_underlying_bits_construct_impl(
          s.random().get_between(0x00U, 0xFFU), s);
    }
}
//------------------------------------------------------------------------------
// operators 1
//------------------------------------------------------------------------------
void bitfield_bit_operators_1(auto& s) {
    eagitest::case_ test{s, 3, "operators 1"};
    eagine::bitfield<test_bit> bf1(test_bit::bit_3);

    test.check(bool(bf1), "is true");
    test.check(!!bf1, "is not false");

    test.check(!bf1.has(test_bit::bit_1), "has not 1 1");
    test.check(!bf1.has(test_bit::bit_2), "has not 1 2");
    test.check(bf1.has(test_bit::bit_3), "has 1 3");
    test.check(!bf1.has(test_bit::bit_4), "has not 1 4");
    test.check(!bf1.has(test_bit::bit_5), "has not 1 5");

    eagine::bitfield<test_bit> bf2(test_bit::bit_1, test_bit::bit_5);

    test.check(bf2.has(test_bit::bit_1), "has 2 1");
    test.check(!bf2.has(test_bit::bit_2), "has not 2 2");
    test.check(!bf2.has(test_bit::bit_3), "has not 2 3");
    test.check(!bf2.has(test_bit::bit_4), "has not 2 4");
    test.check(bf2.has(test_bit::bit_5), "has 2 5");

    eagine::bitfield<test_bit> bf3 = bf2 | test_bit::bit_3;

    test.check(bf3.has(test_bit::bit_1), "has 3 1");
    test.check(!bf3.has(test_bit::bit_2), "has not 3 1");
    test.check(bf3.has(test_bit::bit_3), "has 3 1");
    test.check(!bf3.has(test_bit::bit_4), "has not 3 1");
    test.check(bf3.has(test_bit::bit_5), "has 3 1");

    test.check(!(bf1 == bf2), "not equal 1");
    test.check(!(bf1 == bf3), "not equal 2");
    test.check(bf1 != bf2, "not equal 3");
    test.check(bf1 != bf3, "not equal 4");

    eagine::bitfield<test_bit> bf4 = bf1 | bf2;

    test.check(bf4.has(test_bit::bit_1), "has 4 1");
    test.check(!bf4.has(test_bit::bit_2), "has not 4 2");
    test.check(bf4.has(test_bit::bit_3), "has 4 3");
    test.check(!bf4.has(test_bit::bit_4), "has not 4 4");
    test.check(bf4.has(test_bit::bit_5), "has 4 5");

    test.check(!bool(bf1 & bf2), "and is false");
    test.check(!(bf1 & bf2), "and is not true");

    eagine::bitfield<test_bit> bf5 = bf1 & bf2;

    test.check(!bf5.has(test_bit::bit_1), "has not 5 1");
    test.check(!bf5.has(test_bit::bit_2), "has not 5 2");
    test.check(!bf5.has(test_bit::bit_3), "has not 5 3");
    test.check(!bf5.has(test_bit::bit_4), "has not 5 4");
    test.check(!bf5.has(test_bit::bit_5), "has not 5 5");

    test.check(bool(bf1 & bf3), "and is true");
    test.check(!!(bf1 & bf3), "and is not false");

    eagine::bitfield<test_bit> bf6 = bf1 & bf3;

    test.check(!bf6.has(test_bit::bit_1), "has not 6 1");
    test.check(!bf6.has(test_bit::bit_2), "has not 6 2");
    test.check(bf6.has(test_bit::bit_3), "has 6 3");
    test.check(!bf6.has(test_bit::bit_4), "has not 6 4");
    test.check(!bf6.has(test_bit::bit_5), "has 6 5");
}
//------------------------------------------------------------------------------
// operators 2
//------------------------------------------------------------------------------
void bitfield_bit_operators_2(auto& s) {
    eagitest::case_ test{s, 4, "operators 2"};

    eagine::bitfield<test_bit> bf0;

    test.check(!bf0.has(test_bit::bit_1), "has not 1 1");
    test.check(!bf0.has(test_bit::bit_2), "has not 1 2");
    test.check(!bf0.has(test_bit::bit_3), "has not 1 3");
    test.check(!bf0.has(test_bit::bit_4), "has not 1 4");
    test.check(!bf0.has(test_bit::bit_5), "has not 1 5");

    bf0 |= test_bit::bit_3;

    test.check(!bf0.has(test_bit::bit_1), "has not 2 1");
    test.check(!bf0.has(test_bit::bit_2), "has not 2 2");
    test.check(bf0.has(test_bit::bit_3), "has 2 3");
    test.check(!bf0.has(test_bit::bit_4), "has not 2 4");
    test.check(!bf0.has(test_bit::bit_5), "has not 2 5");

    bf0 |= test_bit::bit_1;

    test.check(bf0.has(test_bit::bit_1), "has 3 1");
    test.check(!bf0.has(test_bit::bit_2), "has not 3 2");
    test.check(bf0.has(test_bit::bit_3), "has 3 3");
    test.check(!bf0.has(test_bit::bit_4), "has not 3 4");
    test.check(!bf0.has(test_bit::bit_5), "has not 3 5");

    bf0 |= test_bit::bit_5;

    test.check(bf0.has(test_bit::bit_1), "has 4 1");
    test.check(!bf0.has(test_bit::bit_2), "has not 4 2");
    test.check(bf0.has(test_bit::bit_3), "has 4 3");
    test.check(!bf0.has(test_bit::bit_4), "has not 4 4");
    test.check(bf0.has(test_bit::bit_5), "has 4 5");

    bf0 |= test_bit::bit_2;
    bf0 |= test_bit::bit_4;

    test.check(bf0.has(test_bit::bit_1), "has 5 1");
    test.check(bf0.has(test_bit::bit_2), "has 5 2");
    test.check(bf0.has(test_bit::bit_3), "has 5 3");
    test.check(bf0.has(test_bit::bit_4), "has 5 4");
    test.check(bf0.has(test_bit::bit_5), "has 5 5");

    bf0 &= test_bit::bit_3;

    test.check(!bf0.has(test_bit::bit_1), "has not 6 1");
    test.check(!bf0.has(test_bit::bit_2), "has not 6 2");
    test.check(bf0.has(test_bit::bit_3), "has 6 3");
    test.check(!bf0.has(test_bit::bit_4), "has not 6 4");
    test.check(!bf0.has(test_bit::bit_5), "has not 6 5");
}
//------------------------------------------------------------------------------
// operators 3
//------------------------------------------------------------------------------
void bitfield_bit_operators_3(auto& s) {
    eagitest::case_ test{s, 5, "operators 3"};

    eagine::bitfield<test_bit> bf0(test_bit::bit_2, test_bit::bit_4);

    test.check(!bf0.has(test_bit::bit_1), "has not 1 1");
    test.check(bf0.has(test_bit::bit_2), "has 1 2");
    test.check(!bf0.has(test_bit::bit_3), "has not 1 3");
    test.check(bf0.has(test_bit::bit_4), "has 1 4");
    test.check(!bf0.has(test_bit::bit_5), "has not 1 5");

    eagine::bitfield<test_bit> bf1 = ~bf0;

    test.check(bf1.has(test_bit::bit_1), "has 2 1");
    test.check(!bf1.has(test_bit::bit_2), "has not 2 2");
    test.check(bf1.has(test_bit::bit_3), "has 2 3");
    test.check(!bf1.has(test_bit::bit_4), "has not 2 4");
    test.check(bf1.has(test_bit::bit_5), "has 2 5");

    eagine::bitfield<test_bit> bfo = bf0 | bf1;

    test.check(bfo.has(test_bit::bit_1), "has 3 1");
    test.check(bfo.has(test_bit::bit_2), "has 3 2");
    test.check(bfo.has(test_bit::bit_3), "has 3 3");
    test.check(bfo.has(test_bit::bit_4), "has 3 4");
    test.check(bfo.has(test_bit::bit_5), "has 3 5");

    eagine::bitfield<test_bit> bfa = bf0 & bf1;

    test.check(!bfa.has(test_bit::bit_1), "has not 4 1");
    test.check(!bfa.has(test_bit::bit_2), "has not 4 2");
    test.check(!bfa.has(test_bit::bit_3), "has not 4 3");
    test.check(!bfa.has(test_bit::bit_4), "has not 4 4");
    test.check(!bfa.has(test_bit::bit_5), "has not 4 5");
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "bitfield", 5};
    test.once(bitfield_default_construct);
    test.repeat(100, bitfield_underlying_bits_construct);
    test.once(bitfield_bit_operators_1);
    test.once(bitfield_bit_operators_2);
    test.once(bitfield_bit_operators_3);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
