/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.memory;
import <algorithm>;
//------------------------------------------------------------------------------
constexpr const unsigned char bmin = 0x02;
constexpr const unsigned char bmax = 0x0F;
//------------------------------------------------------------------------------
// case 1
//------------------------------------------------------------------------------
template <std::size_t N, std::size_t B>
void biteset_N_B(
  eagitest::case_& test,
  std::integral_constant<int, 1>,
  const eagine::biteset<N, B>& bs) {
    using sz_t = typename eagine::biteset<N, B>::size_type;
    test.parameter(N, "N");

    test.check_equal(bs.size(), sz_t(N), "size");

    for(sz_t i = 0; i < bs.size(); ++i) {
        test.check((bs[i] >= bmin) && (bs[i] <= bmax), "in range");
    }
}
//------------------------------------------------------------------------------
// case 2
//------------------------------------------------------------------------------
template <std::size_t N, std::size_t B>
void biteset_N_B(
  eagitest::case_& test,
  std::integral_constant<int, 2>,
  const eagine::biteset<N, B>& bs1,
  const eagine::biteset<N, B>& bs2) {
    test.parameter(N, "N");

    test.check_equal((bs1 == bs2), (bs1.bytes() == bs2.bytes()), "eq");
    test.check_equal((bs1 != bs2), (bs1.bytes() != bs2.bytes()), "ne");
    test.check_equal((bs1 < bs2), (bs1.bytes() < bs2.bytes()), "lt");
    test.check_equal((bs1 <= bs2), (bs1.bytes() <= bs2.bytes()), "le");
    test.check_equal((bs1 > bs2), (bs1.bytes() > bs2.bytes()), "gt");
    test.check_equal((bs1 >= bs2), (bs1.bytes() >= bs2.bytes()), "ge");
}
//------------------------------------------------------------------------------
template <std::size_t B>
void biteset_B(unsigned, auto& test, std::integral_constant<int, 2> c) {
    test.parameter(B, "B");

    using eagine::biteset;
    auto& rg = test.random();

    biteset_N_B(
      test,
      c,
      biteset<1, B>{rg.get_byte(bmin, bmax)},
      biteset<1, B>{rg.get_byte(bmin, bmax)});

    biteset_N_B(
      test,
      c,
      biteset<2, B>{rg.get_byte(bmin, bmax), rg.get_byte(bmin, bmax)},
      biteset<2, B>{rg.get_byte(bmin, bmax), rg.get_byte(bmin, bmax)});

    biteset_N_B(
      test,
      c,
      biteset<4, B>{
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax)},
      biteset<4, B>{
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax)});

    biteset_N_B(
      test,
      c,
      biteset<5, B>{
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax)},
      biteset<5, B>{
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax)});

    biteset_N_B(
      test,
      c,
      biteset<13, B>{
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
      biteset<13, B>{
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
// case 3
//------------------------------------------------------------------------------
template <std::size_t N, std::size_t B, typename T>
void biteset_N_B(
  eagitest::case_& test,
  std::integral_constant<int, 3>,
  eagine::biteset<N, B, T> bs) {
    test.parameter(N, "N");

    using sz_t = typename eagine::biteset<N, B, T>::size_type;

    eagine::biteset<N, B, T> bs2;

    sz_t i;
    for(i = 0; i < bs.size(); ++i) {
        bs2.set(i, bs.get(i));
    }

    i = 0;
    for(T c : bs2) {
        test.check_equal(c, T(bs[i++]), "A");
    }

    test.check(bs == bs2, "are equal");

    for(i = 0; i < bs.size(); ++i) {
        T v = T((i + 1) % (1 << B));
        bs.set(i, v);
        test.check_equal(bs.get(i), v, "B");
    }

    for(i = 0; i < bs.size(); ++i) {
        T v = T((i + 1) % (1 << B));
        bs[i] = v;
        test.check_equal(bs.get(i), v, "C");
    }

    i = 0;
    for(T c : bs) {
        T v = T(++i % (1 << B));
        test.check_equal(c, v, "D");
        v = T((i * 7) % (1 << B));
        c = v;
        test.check_equal(c, v, "E");
    }
}
//------------------------------------------------------------------------------
// case 4
//------------------------------------------------------------------------------
template <std::size_t N, std::size_t B, typename T>
void biteset_N_B(
  eagitest::case_& test,
  std::integral_constant<int, 4>,
  eagine::biteset<N, B, T> bs) {
    test.parameter(N, "N");

    std::sort(bs.begin(), bs.end());

    const eagine::biteset<N, B, T>& cbs = bs;

    test.check(std::is_sorted(cbs.begin(), cbs.end()), "is sorted");
}
//------------------------------------------------------------------------------
// boilerplate
//------------------------------------------------------------------------------
template <std::size_t B, int C>
void biteset_B(unsigned, auto& test, std::integral_constant<int, C> c) {
    test.parameter(B, "B");

    using eagine::biteset;
    auto& rg = test.random();

    biteset_N_B(test, c, biteset<1, B>{rg.get_byte(bmin, bmax)});
    biteset_N_B(
      test, c, biteset<2, B>{rg.get_byte(bmin, bmax), rg.get_byte(bmin, bmax)});
    biteset_N_B(
      test,
      c,
      biteset<3, B>{
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax)});
    biteset_N_B(
      test,
      c,
      biteset<4, B>{
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax)});
    biteset_N_B(
      test,
      c,
      biteset<8, B>{
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax),
        rg.get_byte(bmin, bmax)});
    biteset_N_B(
      test,
      c,
      biteset<11, B>{
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
    biteset_N_B(
      test,
      c,
      biteset<16, B>{
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
template <int C>
void biteset_C(unsigned run, auto& test, std::integral_constant<int, C> c = {}) {
    test.parameter(C, "C");
    biteset_B<4>(run, test, c);
    biteset_B<5>(run, test, c);
    biteset_B<6>(run, test, c);
    biteset_B<7>(run, test, c);
    biteset_B<8>(run, test, c);
    biteset_B<9>(run, test, c);
    biteset_B<11>(run, test, c);
    biteset_B<15>(run, test, c);
}
//------------------------------------------------------------------------------
void biteset_1(unsigned run, auto& s) {
    eagitest::case_ test{s, "1"};
    return biteset_C<1>(run, test);
}
//------------------------------------------------------------------------------
void biteset_2(unsigned run, auto& s) {
    eagitest::case_ test{s, "2"};
    return biteset_C<2>(run, test);
}
//------------------------------------------------------------------------------
void biteset_3(unsigned run, auto& s) {
    eagitest::case_ test{s, "3"};
    return biteset_C<3>(run, test);
}
//------------------------------------------------------------------------------
void biteset_4(unsigned run, auto& s) {
    eagitest::case_ test{s, "sort"};
    return biteset_C<4>(run, test);
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "biteset"};
    test.repeat(100, biteset_1);
    test.repeat(100, biteset_2);
    test.repeat(100, biteset_3);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
