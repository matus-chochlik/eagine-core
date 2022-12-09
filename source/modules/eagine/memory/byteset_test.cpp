/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.memory;
//------------------------------------------------------------------------------
constexpr const unsigned char bmin = 0x04;
constexpr const unsigned char bmax = 0x3F;
//------------------------------------------------------------------------------
// case 1
//------------------------------------------------------------------------------
template <std::size_t N>
void byteset_N(
  eagitest::case_& test,
  std::integral_constant<int, 1>,
  const eagine::byteset<N>& bs) {
    using sz_t = typename eagine::byteset<N>::size_type;

    test.check_equal(bs.size(), sz_t(N), "size");

    for(auto& b : bs) {
        test.check((b >= bmin) && (b <= bmax), "in limits 1");
    }

    using iter = typename eagine::byteset<N>::const_iterator;
    for(iter i = bs.begin(); i != bs.end(); ++i) {
        test.check((*i >= bmin) && (*i <= bmax), "in limits 2");
    }

    using sz_t = typename eagine::byteset<N>::size_type;
    for(sz_t i = 0; i < bs.size(); ++i) {
        test.check((bs[i] >= bmin) && (bs[i] <= bmax), "in limits 3");
    }
}
//------------------------------------------------------------------------------
// case 2
//------------------------------------------------------------------------------
template <std::size_t N>
void byteset_N(
  eagitest::case_& test,
  std::integral_constant<int, 2>,
  const eagine::byteset<N>& bs1,
  const eagine::byteset<N>& bs2) {

    test.check_equal(
      (bs1 == bs2), (std::memcmp(bs1.data(), bs2.data(), N) == 0), "eq");
    test.check_equal(
      (bs1 != bs2), (std::memcmp(bs1.data(), bs2.data(), N) != 0), "nq");
    test.check_equal(
      (bs1 < bs2), (std::memcmp(bs1.data(), bs2.data(), N) < 0), "lt");
    test.check_equal(
      (bs1 <= bs2), (std::memcmp(bs1.data(), bs2.data(), N) <= 0), "le");
    test.check_equal(
      (bs1 > bs2), (std::memcmp(bs1.data(), bs2.data(), N) > 0), "gt");
    test.check_equal(
      (bs1 >= bs2), (std::memcmp(bs1.data(), bs2.data(), N) >= 0), "ge");
}
//------------------------------------------------------------------------------
template <int C>
void byteset_C(unsigned, auto& test, std::integral_constant<int, C> c = {}) {

    using eagine::byteset;
    auto& rg = test.random();

    byteset_N(test, c, byteset<1>{rg.get_byte(bmin, bmax)});
    byteset_N(
      test, c, byteset<2>{rg.get_byte(bmin, bmax), rg.get_byte(bmin, bmax)});
    byteset_N(
      test,
      c,
      byteset<3>{
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax)});
    byteset_N(
      test,
      c,
      byteset<4>{
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax)});
    byteset_N(
      test,
      c,
      byteset<5>{
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax)});
    byteset_N(
      test,
      c,
      byteset<8>{
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax)});
    byteset_N(
      test,
      c,
      byteset<11>{
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax)});
    byteset_N(
      test,
      c,
      byteset<13>{
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax)});
    byteset_N(
      test,
      c,
      byteset<16>{
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax)});
}
//------------------------------------------------------------------------------
void byteset_C(unsigned, auto& test, std::integral_constant<int, 2> c = {}) {

    using eagine::byteset;
    auto& rg = test.random();

    byteset_N(
      test,
      c,
      byteset<1>{rg.get_byte(bmin, bmax)},
      byteset<1>{rg.get_byte(bmin, bmax)});
    byteset_N(
      test,
      c,
      byteset<2>{rg.get_byte(bmin, bmax), rg.get_byte(bmin, bmax)},
      byteset<2>{rg.get_byte(bmin, bmax), rg.get_byte(bmin, bmax)});
    byteset_N(
      test,
      c,
      byteset<3>{
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax)},
      byteset<3>{
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax)});
    byteset_N(
      test,
      c,
      byteset<4>{
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax)},
      byteset<4>{
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax)});
    byteset_N(
      test,
      c,
      byteset<5>{
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax)},
      byteset<5>{
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax)});
    byteset_N(
      test,
      c,
      byteset<8>{
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax)},
      byteset<8>{
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax)});
    byteset_N(
      test,
      c,
      byteset<11>{
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax)},
      byteset<11>{
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax)});
    byteset_N(
      test,
      c,
      byteset<15>{
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax)},
      byteset<15>{
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax)});
    byteset_N(
      test,
      c,
      byteset<16>{
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax)},
      byteset<16>{
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax)});
}
//------------------------------------------------------------------------------
void byteset_1(unsigned run, auto& s) {
    eagitest::case_ test{s, 1, "1"};
    return byteset_C(run, test, std::integral_constant<int, 1>{});
}
//------------------------------------------------------------------------------
void byteset_2(unsigned run, auto& s) {
    eagitest::case_ test{s, 2, "2"};
    return byteset_C(run, test, std::integral_constant<int, 2>{});
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "byteset", 2};
    test.repeat(1000, byteset_1);
    test.repeat(1000, byteset_2);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
