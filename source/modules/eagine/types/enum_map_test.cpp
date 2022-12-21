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
enum class test_enum : unsigned {
    value_a = 1U << 0U,
    value_b = 1U << 1U,
    value_c = 1U << 2U,
    value_d = 1U << 3U,
    value_e = 1U << 4U,
    value_f = 1U << 5U
};

using test_bits = eagine::bitfield<test_enum>;

static test_bits operator|(test_enum a, test_enum b) noexcept {
    return {a, b};
}

template <test_enum>
class test_enum_map_counter {
public:
    using value_type = int;

    constexpr value_type value() const noexcept {
        return _c;
    }

    template <typename... P>
    constexpr void operator()(const P&...) noexcept {
        ++_c;
    }

private:
    int _c{0};
};

template <test_enum>
using test_enum_map_counter_2 = std::tuple<test_enum, int>;
//------------------------------------------------------------------------------
void enum_map_1(eagitest::suite& s) {
    eagitest::case_ test{s, 1, "1"};
    eagitest::track trck{test, 0, 2};

    eagine::static_enum_map<
      test_enum,
      test_enum_map_counter,
      test_enum::value_a,
      test_enum::value_b,
      test_enum::value_c,
      test_enum::value_d,
      test_enum::value_e>
      counters;

    const auto incr = [&](auto, auto& ctr) {
        ctr();
        trck.passed_part(1);
    };

    const int n = 100;
    for(int i = 0; i < n; ++i) {
        test.check(counters.visit(test_enum::value_d, incr), "visit a");
        test.check(counters.visit(test_enum::value_a, incr), "visit b");
        test.check(counters.visit(test_enum::value_c, incr), "visit c");
        test.check(counters.visit(test_enum::value_e, incr), "visit d");
        test.check(counters.visit(test_enum::value_b, incr), "visit e");
        test.check(not counters.visit(test_enum::value_f, incr), "not visit f");
        trck.passed_part(2);
    }

    const auto& ccounters = counters;

    test.check_equal(ccounters.get<test_enum::value_a>().value(), n, "a is ok");
    test.check_equal(ccounters.get<test_enum::value_b>().value(), n, "b is ok");
    test.check_equal(ccounters.get<test_enum::value_c>().value(), n, "c is ok");
    test.check_equal(ccounters.get<test_enum::value_d>().value(), n, "d is ok");
    test.check_equal(ccounters.get<test_enum::value_e>().value(), n, "e is ok");
}
//------------------------------------------------------------------------------
void enum_map_2(eagitest::suite& s) {
    eagitest::case_ test{s, 2, "2"};
    eagitest::track trck{test, 0, 2};

    eagine::static_enum_map<
      test_enum,
      test_enum_map_counter,
      test_enum::value_a,
      test_enum::value_b,
      test_enum::value_c,
      test_enum::value_d,
      test_enum::value_e>
      counters;

    const int n = 100;
    for(int i = 0; i < n; ++i) {
        counters.get<test_enum::value_a>()();
        counters.get<test_enum::value_b>()();
        counters.get<test_enum::value_c>()();
        counters.get<test_enum::value_d>()();
        counters.get<test_enum::value_e>()();
        trck.passed_part(1);
    }

    const auto& ccounters = counters;

    const auto chk_value = [&](auto, const auto& ctr) {
        test.check_equal(ctr.value(), n, "value is ok");
        trck.passed_part(2);
    };

    test.check(ccounters.visit(test_enum::value_a, chk_value), "visit a");
    test.check(ccounters.visit(test_enum::value_b, chk_value), "visit b");
    test.check(ccounters.visit(test_enum::value_c, chk_value), "visit c");
    test.check(ccounters.visit(test_enum::value_d, chk_value), "visit d");
    test.check(ccounters.visit(test_enum::value_e, chk_value), "visit e");
    test.check(
      not ccounters.visit(test_enum::value_f, chk_value), "not visit f");
}
//------------------------------------------------------------------------------
void enum_map_3(eagitest::suite& s) {
    eagitest::case_ test{s, 3, "3"};
    eagitest::track trck{test, 0, 6};

    eagine::static_enum_map<
      test_enum,
      test_enum_map_counter,
      test_enum::value_a,
      test_enum::value_b,
      test_enum::value_c,
      test_enum::value_d,
      test_enum::value_e>
      counters;

    const auto incr = [&](auto, auto& ctr) {
        ctr();
        trck.passed_part(1);
    };

    const int n = 100;
    for(int i = 0; i < n; ++i) {
        test.check(counters.visit(test_enum::value_a, incr), "visit a");
        trck.passed_part(2);
    }

    for(int i = 0; i < n; ++i) {
        test.check(
          counters.visit(test_enum::value_a | test_enum::value_b, incr),
          "visit ab");
        trck.passed_part(3);
    }

    for(int i = 0; i < n; ++i) {
        test.check(
          counters.visit(
            test_enum::value_a | test_enum::value_b | test_enum::value_c, incr),
          "visit abc");
        trck.passed_part(4);
    }

    for(int i = 0; i < n; ++i) {
        test.check(
          counters.visit(
            test_enum::value_a | test_enum::value_b | test_enum::value_c |
              test_enum::value_d,
            incr),
          "visit abcd");
        trck.passed_part(5);
    }

    for(int i = 0; i < n; ++i) {
        test.check(
          counters.visit(
            test_enum::value_a | test_enum::value_b | test_enum::value_c |
              test_enum::value_d | test_enum::value_e,
            incr),
          "visit abcde");
        trck.passed_part(6);
    }

    const auto& ccounters = counters;

    test.check_equal(ccounters.get<test_enum::value_a>().value(), n * 5, "a");
    test.check_equal(ccounters.get<test_enum::value_b>().value(), n * 4, "b");
    test.check_equal(ccounters.get<test_enum::value_c>().value(), n * 3, "c");
    test.check_equal(ccounters.get<test_enum::value_d>().value(), n * 2, "d");
    test.check_equal(ccounters.get<test_enum::value_e>().value(), n * 1, "e");
}
//------------------------------------------------------------------------------
void enum_map_4(eagitest::suite& s) {
    eagitest::case_ test{s, 4, "4"};
    eagitest::track trck{test, 0, 4};

    eagine::static_enum_map<
      test_enum,
      test_enum_map_counter_2,
      test_enum::value_a,
      test_enum::value_b,
      test_enum::value_c,
      test_enum::value_d,
      test_enum::value_e>
      counters;

    auto init = [&](auto val, auto& tup) {
        std::get<0>(tup) = val;
        std::get<1>(tup) = 1;
        trck.passed_part(1);
    };

    test.check(counters.visit_all(init), "init ok");

    const auto incr = [&](auto, auto& tup) {
        std::get<1>(tup)++;
        trck.passed_part(2);
    };

    const int n = 100;
    for(int i = 0; i < n; ++i) {
        test.check(counters.visit(test_enum::value_d, incr), "visit d");
        test.check(counters.visit(test_enum::value_a, incr), "visit a");
        test.check(counters.visit(test_enum::value_c, incr), "visit c");
        test.check(counters.visit(test_enum::value_e, incr), "visit e");
        test.check(counters.visit(test_enum::value_b, incr), "visit b");
        test.check(not counters.visit(test_enum::value_f, incr), "not visit f");
        trck.passed_part(3);
    }

    const auto& ccounters = counters;

    test.check(
      std::get<0>(ccounters.get<test_enum::value_a>()) == test_enum::value_a,
      "a");
    test.check(
      std::get<0>(ccounters.get<test_enum::value_b>()) == test_enum::value_b,
      "b");
    test.check(
      std::get<0>(ccounters.get<test_enum::value_c>()) == test_enum::value_c,
      "c");
    test.check(
      std::get<0>(ccounters.get<test_enum::value_d>()) == test_enum::value_d,
      "d");
    test.check(
      std::get<0>(ccounters.get<test_enum::value_e>()) == test_enum::value_e,
      "e");

    const auto chk_tuple = [&](auto value, const auto& tup) {
        test.check(std::get<0>(tup) == value, "get 0 ok");
        test.check(std::get<1>(tup) == n + 1, "get 1 ok");
        trck.passed_part(4);
    };

    test.check(ccounters.visit(test_enum::value_a, chk_tuple), "check a");
    test.check(ccounters.visit(test_enum::value_b, chk_tuple), "check b");
    test.check(ccounters.visit(test_enum::value_c, chk_tuple), "check c");
    test.check(ccounters.visit(test_enum::value_d, chk_tuple), "check d");
    test.check(ccounters.visit(test_enum::value_e, chk_tuple), "check e");
    test.check(
      not ccounters.visit(test_enum::value_f, chk_tuple), "not check f");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "enum_map", 4};
    test.once(enum_map_1);
    test.once(enum_map_2);
    test.once(enum_map_3);
    test.once(enum_map_4);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
