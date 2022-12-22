/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.container;
import <functional>;
import <string>;
//------------------------------------------------------------------------------
using int_iterator = eagine::selfref_iterator<int>;

using strint_iterator = eagine::transforming_iterator<
  int_iterator,
  std::string,
  std::string,
  std::function<std::string(int)>>;
//------------------------------------------------------------------------------
void iterator_1(auto& s) {
    eagitest::case_ test{s, 1, "1"};
    auto& rg{test.random()};

    const int bi = rg.get_int(0, 9);
    const int ei = bi + 1 + rg.get_int(0, 9);
    int ii = bi;

    const int_iterator b{bi};
    const int_iterator e{ei};
    int_iterator i = b;

    test.check(b == i, "begin ok 1");
    test.check(e != i, "end ok 1");

    while(i < e) {
        test.check(b <= i, "begin ok 2");

        test.check_equal(i - b, ii - bi, "distance from begin 1");
        test.check_equal(e - i, ei - ii, "distance from end 1");

        test.check(int_iterator(ii++) == i++, "increment 1");

        test.check_equal(i - b, ii - bi, "distance from begin 2");
        test.check_equal(e - i, ei - ii, "distance from end 2");

        test.check(e >= i, "end ok 2");
    }

    test.check(b != i, "begin ok 3");
    test.check(e == i, "end ok 3");

    while(i > b) {
        test.check(e >= i, "end ok 4");

        test.check_equal(i - b, ii - bi, "distance from begin 3");
        test.check_equal(e - i, ei - ii, "distance from end 3");

        test.check(int_iterator(ii--) == i--, "decrement 1");

        test.check_equal(i - b, ii - bi, "distance from begin 4");
        test.check_equal(e - i, ei - ii, "distance from end 4");

        test.check(i >= b, "begin ok 4");
    }

    test.check(b == i, "begin ok 5");
    test.check(e != i, "end ok 5");

    while(i < e) {
        test.check(++ii == *(++i), "increment 2");
    }

    test.check(b != i, "begin ok 5");
    test.check(e == i, "end ok 5");

    while(b < i) {
        test.check(--ii == *(--i), "decrement 2");
    }
}
//------------------------------------------------------------------------------
void iterator_2(auto& s) {
    eagitest::case_ test{s, 2, "2"};
    auto& rg{test.random()};

    const int bi = rg.get_int(0, 1000);
    const int ei = bi + 1 + rg.get_int(0, 1000);
    int ii = bi;
    std::function<std::string(int)> test_transform(
      static_cast<std::string (*)(int)>(&std::to_string));

    const strint_iterator b(bi, test_transform);
    const strint_iterator e(ei, test_transform);
    strint_iterator i = b;

    test.check(b == i, "begin ok 1");
    test.check(e != i, "end ok 1");

    while(i < e) {
        test.check(b <= i, "begin ok 2");

        test.check_equal(i - b, ii - bi, "distance from begin 1");
        test.check_equal(e - i, ei - ii, "distance from end 1");

        test.check(int_iterator(ii++) == i++, "increment 1");

        test.check_equal(i - b, ii - bi, "distance from begin 2");
        test.check_equal(e - i, ei - ii, "distance from end 2");

        test.check(e >= i, "end ok 2");
    }

    test.check(b != i, "begin ok 3");
    test.check(e == i, "end ok 3");

    while(i > b) {
        test.check(e >= i, "end ok 4");

        test.check_equal(i - b, ii - bi, "distance from begin 3");
        test.check_equal(e - i, ei - ii, "distance from end 3");

        test.check(int_iterator(ii--) == i--, "decrement 1");

        test.check_equal(i - b, ii - bi, "distance from begin 4");
        test.check_equal(e - i, ei - ii, "distance from end 4");

        test.check(i >= b, "begin ok 4");
    }

    test.check(b == i, "begin ok 5");
    test.check(e != i, "end ok 5");

    while(i < e) {
        test.check(std::to_string(++ii) == *(++i), "increment 2");
    }

    test.check(b != i, "begin ok 5");
    test.check(e == i, "end ok 5");

    while(b < i) {
        test.check(std::to_string(--ii) == *(--i), "decrement 2");
    }
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "iterator", 2};
    test.once(iterator_1);
    test.once(iterator_2);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
