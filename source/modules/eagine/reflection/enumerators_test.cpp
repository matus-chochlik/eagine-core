/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.reflection;
//------------------------------------------------------------------------------
namespace eagine {
enum class test_enum_1 : unsigned { a, b, c, d, e, f, g, h, i, j };

template <>
struct enumerator_traits<test_enum_1> {
    static constexpr auto mapping() noexcept {
        return enumerator_map_type<test_enum_1, 10>{
          {{"a", test_enum_1::a},
           {"b", test_enum_1::b},
           {"c", test_enum_1::c},
           {"d", test_enum_1::d},
           {"e", test_enum_1::e},
           {"f", test_enum_1::f},
           {"g", test_enum_1::g},
           {"h", test_enum_1::h},
           {"i", test_enum_1::i},
           {"j", test_enum_1::j}}};
    }
};
//------------------------------------------------------------------------------
enum class test_enum_2 : unsigned { one = 1U, two = 2U, three = 3U, four = 4U };

template <>
struct enumerator_traits<test_enum_2> {
    static constexpr auto mapping() noexcept {
        return enumerator_map_type<test_enum_2, 4>{
          {{"one", test_enum_2::one},
           {"two", test_enum_2::two},
           {"three", test_enum_2::three},
           {"four", test_enum_2::four}}};
    }
};
//------------------------------------------------------------------------------
enum class test_enum_3 : unsigned { one = 1U, two = 2U, four = 4U, eight = 8U };

template <>
struct enumerator_traits<test_enum_3> {
    static constexpr auto mapping() noexcept {
        return enumerator_map_type<test_enum_3, 4>{
          {{"one", test_enum_3::one},
           {"two", test_enum_3::two},
           {"four", test_enum_3::four},
           {"eight", test_enum_3::eight}}};
    }
};
} // namespace eagine
//------------------------------------------------------------------------------
// is consecutive
//------------------------------------------------------------------------------
void enum_is_consecutive(auto& s) {
    using eagine::is_consecutive;
    eagitest::case_ test{s, 1, "is consecutive"};

    test.check_equal(is_consecutive<eagine::test_enum_1>(), true, "1");
    test.check_equal(is_consecutive<eagine::test_enum_2>(), false, "2");
    test.check_equal(is_consecutive<eagine::test_enum_3>(), false, "3");
}
//------------------------------------------------------------------------------
// is bitset
//------------------------------------------------------------------------------
void enum_is_bitset(auto& s) {
    using eagine::is_bitset;
    eagitest::case_ test{s, 2, "is bitset"};

    test.check_equal(is_bitset<eagine::test_enum_1>(), false, "1");
    test.check_equal(is_bitset<eagine::test_enum_2>(), false, "2");
    test.check_equal(is_bitset<eagine::test_enum_3>(), true, "3");
}
//------------------------------------------------------------------------------
// enumerator count
//------------------------------------------------------------------------------
void enum_enumerator_count(auto& s) {
    using eagine::enumerator_count;
    eagitest::case_ test{s, 3, "enumerator count"};

    test.check_equal(enumerator_count<eagine::test_enum_1>(), 10, "1");
    test.check_equal(enumerator_count<eagine::test_enum_2>(), 4, "2");
    test.check_equal(enumerator_count<eagine::test_enum_3>(), 4, "3");
}
//------------------------------------------------------------------------------
// enumerator index
//------------------------------------------------------------------------------
void enum_enumerator_index(auto& s) {
    using eagine::enumerator_index;
    eagitest::case_ test{s, 4, "enumerator index"};

    test.check_equal(enumerator_index(eagine::test_enum_1::a), 0U, "1::a");
    test.check_equal(enumerator_index(eagine::test_enum_1::b), 1U, "1::b");
    test.check_equal(enumerator_index(eagine::test_enum_1::c), 2U, "1::c");
    test.check_equal(enumerator_index(eagine::test_enum_1::d), 3U, "1::d");
    test.check_equal(enumerator_index(eagine::test_enum_1::e), 4U, "1::e");
    test.check_equal(enumerator_index(eagine::test_enum_1::f), 5U, "1::f");
    test.check_equal(enumerator_index(eagine::test_enum_1::g), 6U, "1::g");
    test.check_equal(enumerator_index(eagine::test_enum_1::h), 7U, "1::h");
    test.check_equal(enumerator_index(eagine::test_enum_1::i), 8U, "1::i");
    test.check_equal(enumerator_index(eagine::test_enum_1::j), 9U, "1::j");

    test.check_equal(enumerator_index(eagine::test_enum_2::one), 0U, "2::1");
    test.check_equal(enumerator_index(eagine::test_enum_2::two), 1U, "2::2");
    test.check_equal(enumerator_index(eagine::test_enum_2::three), 2U, "2::3");
    test.check_equal(enumerator_index(eagine::test_enum_2::four), 3U, "2::4");

    test.check_equal(enumerator_index(eagine::test_enum_3::one), 0U, "3::1");
    test.check_equal(enumerator_index(eagine::test_enum_3::two), 1U, "3::2");
    test.check_equal(enumerator_index(eagine::test_enum_3::four), 2U, "3::4");
    test.check_equal(enumerator_index(eagine::test_enum_3::eight), 3U, "3::8");
}
//------------------------------------------------------------------------------
// enumerator name
//------------------------------------------------------------------------------
void enum_enumerator_name(auto& s) {
    using eagine::enumerator_name;
    eagitest::case_ test{s, 5, "enumerator name"};

    const auto sw = [](const eagine::string_view& v) -> auto& {
        return v;
    };

    test.check_equal(sw(enumerator_name(eagine::test_enum_1::a)), "a", "1::a");
    test.check_equal(sw(enumerator_name(eagine::test_enum_1::b)), "b", "1::b");
    test.check_equal(sw(enumerator_name(eagine::test_enum_1::c)), "c", "1::c");
    test.check_equal(sw(enumerator_name(eagine::test_enum_1::d)), "d", "1::d");
    test.check_equal(sw(enumerator_name(eagine::test_enum_1::e)), "e", "1::e");
    test.check_equal(sw(enumerator_name(eagine::test_enum_1::f)), "f", "1::f");
    test.check_equal(sw(enumerator_name(eagine::test_enum_1::g)), "g", "1::g");
    test.check_equal(sw(enumerator_name(eagine::test_enum_1::h)), "h", "1::h");
    test.check_equal(sw(enumerator_name(eagine::test_enum_1::i)), "i", "1::i");
    test.check_equal(sw(enumerator_name(eagine::test_enum_1::j)), "j", "1::j");

    test.check_equal(
      sw(enumerator_name(eagine::test_enum_2::one)), "one", "2::one");
    test.check_equal(
      sw(enumerator_name(eagine::test_enum_2::two)), "two", "2::two");
    test.check_equal(
      sw(enumerator_name(eagine::test_enum_2::three)), "three", "2::three");
    test.check_equal(
      sw(enumerator_name(eagine::test_enum_2::four)), "four", "2::four");

    test.check_equal(
      sw(enumerator_name(eagine::test_enum_3::one)), "one", "3::one");
    test.check_equal(
      sw(enumerator_name(eagine::test_enum_3::two)), "two", "3::two");
    test.check_equal(
      sw(enumerator_name(eagine::test_enum_3::four)), "four", "3::four");
    test.check_equal(
      sw(enumerator_name(eagine::test_enum_3::eight)), "eight", "3::eight");
}
//------------------------------------------------------------------------------
// from string
//------------------------------------------------------------------------------
void enum_from_string(auto& s) {
    using eagine::default_selector;
    using eagine::from_string;
    eagitest::case_ test{s, 6, "from string"};

    test.check(
      eagine::test_enum_1::a == from_string<eagine::test_enum_1>("a"), "1::a");
    test.check(
      eagine::test_enum_1::b == from_string<eagine::test_enum_1>("b"), "1::b");
    test.check(
      eagine::test_enum_1::c == from_string<eagine::test_enum_1>("c"), "1::c");
    test.check(
      eagine::test_enum_1::d == from_string<eagine::test_enum_1>("d"), "1::d");
    test.check(
      eagine::test_enum_1::e == from_string<eagine::test_enum_1>("e"), "1::e");
    test.check(
      eagine::test_enum_1::f == from_string<eagine::test_enum_1>("f"), "1::f");
    test.check(
      eagine::test_enum_1::g == from_string<eagine::test_enum_1>("g"), "1::g");
    test.check(
      eagine::test_enum_1::h == from_string<eagine::test_enum_1>("h"), "1::h");
    test.check(
      eagine::test_enum_1::i == from_string<eagine::test_enum_1>("i"), "1::i");
    test.check(
      eagine::test_enum_1::j == from_string<eagine::test_enum_1>("j"), "1::j");

    test.check(
      eagine::test_enum_2::one == from_string<eagine::test_enum_2>("one"),
      "2::one");
    test.check(
      eagine::test_enum_2::two == from_string<eagine::test_enum_2>("two"),
      "2::two");
    test.check(
      eagine::test_enum_2::three == from_string<eagine::test_enum_2>("three"),
      "2::three");
    test.check(
      eagine::test_enum_2::four == from_string<eagine::test_enum_2>("four"),
      "2::four");
}
//------------------------------------------------------------------------------
// for each 1
//------------------------------------------------------------------------------
void enum_for_each_1(auto& s) {
    using eagine::default_selector;
    using eagine::from_string;
    eagitest::case_ test{s, 7, "for each 1"};
    eagitest::track trck{test, 10, 1};

    eagine::for_each_enumerator(
      [&test, &trck](const auto& info) {
          test.check(not info.name.empty(), "has name");
          trck.checkpoint(1);
      },
      std::type_identity<eagine::test_enum_1>{});
}
//------------------------------------------------------------------------------
// for each 2
//------------------------------------------------------------------------------
void enum_for_each_2(auto& s) {
    using eagine::default_selector;
    using eagine::from_string;
    eagitest::case_ test{s, 8, "for each 2"};
    eagitest::track trck{test, 4, 1};

    eagine::for_each_enumerator(
      [&test, &trck](const auto& info) {
          test.check(not info.name.empty(), "has name");
          trck.checkpoint(1);
      },
      std::type_identity<eagine::test_enum_2>{});
}
//------------------------------------------------------------------------------
// for each 3
//------------------------------------------------------------------------------
void enum_for_each_3(auto& s) {
    using eagine::default_selector;
    using eagine::from_string;
    eagitest::case_ test{s, 9, "for each 3"};
    eagitest::track trck{test, 4, 1};

    eagine::for_each_enumerator(
      [&test, &trck](const auto& info) {
          test.check(not info.name.empty(), "has name");
          trck.checkpoint(1);
      },
      std::type_identity<eagine::test_enum_3>{});
}
//------------------------------------------------------------------------------
// roundtrip 1
//------------------------------------------------------------------------------
void enum_roundtrip_1(auto& s) {
    using eagine::default_selector;
    using eagine::from_string;
    eagitest::case_ test{s, 10, "roundtrip 1"};
    eagitest::track trck{test, 10, 1};

    const std::type_identity<eagine::test_enum_1> tid;
    for(const auto& info : eagine::enumerators(tid)) {
        test.check(
          info.enumerator == from_string(info.name, tid), "same value");
        test.check(
          info.name == enumerator_name(info.enumerator, tid), "same name");
        trck.checkpoint(1);
    }
}
//------------------------------------------------------------------------------
// roundtrip 2
//------------------------------------------------------------------------------
void enum_roundtrip_2(auto& s) {
    using eagine::default_selector;
    using eagine::from_string;
    eagitest::case_ test{s, 11, "roundtrip 2"};
    eagitest::track trck{test, 4, 1};

    const std::type_identity<eagine::test_enum_2> tid;
    for(const auto& info : eagine::enumerators(tid)) {
        test.check(
          info.enumerator == from_string(info.name, tid), "same value");
        test.check(
          info.name == enumerator_name(info.enumerator, tid), "same name");
        trck.checkpoint(1);
    }
}
//------------------------------------------------------------------------------
// roundtrip 3
//------------------------------------------------------------------------------
void enum_roundtrip_3(auto& s) {
    using eagine::default_selector;
    using eagine::from_string;
    eagitest::case_ test{s, 12, "roundtrip 3"};
    eagitest::track trck{test, 4, 1};

    const std::type_identity<eagine::test_enum_3> tid;
    for(const auto& info : eagine::enumerators(tid)) {
        test.check(
          info.enumerator == from_string(info.name, tid), "same value");
        test.check(
          info.name == enumerator_name(info.enumerator, tid), "same name");
        trck.checkpoint(1);
    }
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "enumerators", 12};
    test.once(enum_is_consecutive);
    test.once(enum_is_bitset);
    test.once(enum_enumerator_count);
    test.once(enum_enumerator_index);
    test.once(enum_enumerator_name);
    test.once(enum_from_string);
    test.once(enum_for_each_1);
    test.once(enum_for_each_2);
    test.once(enum_for_each_3);
    test.once(enum_roundtrip_1);
    test.once(enum_roundtrip_2);
    test.once(enum_roundtrip_3);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
