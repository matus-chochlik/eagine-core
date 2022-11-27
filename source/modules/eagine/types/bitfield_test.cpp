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
void bitfield_default_construct(auto& s) {
    eagitest::case_ test{s, "default construct"};
    eagine::bitfield<test_bit> b;
    test.constructed(b, "b");
    test.check(b.is_empty(), "is empty");
    test.check(!b, "is false");
    //
    for(auto [idx, bit] : test_bit_list()) {
        test.check(!b.has(bit), "!has(bit_{})", idx);
    }
    //
    for(auto [idx, bit] : test_bit_list()) {
        test.check(b.has_not(bit), "has_not(bit_{}) bit", idx);
    }
    //
    for(auto [idx, bit] : test_bit_list()) {
        test.check(!b.has_only(bit), "!has_only(bit_{})", idx);
    }
    //
    for(auto [idx, bit] : test_bit_list()) {
        test.check(b.has_at_most(bit), "has_at_most(bit_{})", idx);
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
void bitfield_underlying_bits_construct_impl(unsigned u, auto& s) {
    eagitest::case_ test{s, "underlying construct"};
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
    }
    //
    for(auto [idx, bit] : test_bit_list()) {
        test.check(
          b.has_not(bit) == (((u >> idx) & 0x1U) != 0x1U),
          "has_not(bit_{}) bit",
          idx);
    }
    //
    for(auto [idx, bit] : test_bit_list()) {
        test.check(
          b.has_only(bit) ==
            ((((~(1U << idx) & 0xFFU) & u) == 0x00U) && (u != 0U)),
          "has_only(bit_{})",
          idx);
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
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "bitfield"};
    test.once(bitfield_default_construct);
    test.repeat(100, bitfield_underlying_bits_construct);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
