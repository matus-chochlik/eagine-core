/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.types;
import eagine.core.memory;
import eagine.core.reflection;
//------------------------------------------------------------------------------
namespace eagine {
enum class test_enum_1 : unsigned { a, b, c, d, e, f, g, h, i, j };

template <typename Selector>
constexpr auto enumerator_mapping(
  std::type_identity<test_enum_1>,
  Selector) noexcept {
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
//------------------------------------------------------------------------------
enum class test_enum_2 : unsigned { one = 1U, two = 2U, three = 3U, four = 4U };

template <typename Selector>
constexpr auto enumerator_mapping(
  std::type_identity<test_enum_2>,
  Selector) noexcept {
    return enumerator_map_type<test_enum_2, 4>{
      {{"one", test_enum_2::one},
       {"two", test_enum_2::two},
       {"three", test_enum_2::three},
       {"four", test_enum_2::four}}};
}
//------------------------------------------------------------------------------
enum class test_enum_3 : unsigned { one = 1U, two = 2U, four = 4U, eight = 8U };

template <typename Selector>
constexpr auto enumerator_mapping(
  std::type_identity<test_enum_3>,
  Selector) noexcept {
    return enumerator_map_type<test_enum_3, 4>{
      {{"one", test_enum_3::one},
       {"two", test_enum_3::two},
       {"four", test_enum_3::four},
       {"eight", test_enum_3::eight}}};
}
} // namespace eagine
//------------------------------------------------------------------------------
// is consecutive
//------------------------------------------------------------------------------
void enum_is_consecutive(auto& s) {
    using eagine::is_consecutive;
    eagitest::case_ test{s, 1, "is consecutive"};

    test.check_equal(
      is_consecutive(std::type_identity<eagine::test_enum_1>()), true, "1");
    test.check_equal(
      is_consecutive(std::type_identity<eagine::test_enum_2>()), false, "2");
    test.check_equal(
      is_consecutive(std::type_identity<eagine::test_enum_3>()), false, "3");
}
//------------------------------------------------------------------------------
// enumerator count
//------------------------------------------------------------------------------
void enum_enumerator_count(auto& s) {
    using eagine::enumerator_count;
    eagitest::case_ test{s, 2, "enumerator count"};

    test.check_equal(
      enumerator_count(std::type_identity<eagine::test_enum_1>()), 10, "1");
    test.check_equal(
      enumerator_count(std::type_identity<eagine::test_enum_2>()), 4, "2");
    test.check_equal(
      enumerator_count(std::type_identity<eagine::test_enum_3>()), 4, "3");
}
//------------------------------------------------------------------------------
// enumerator name
//------------------------------------------------------------------------------
void enum_enumerator_name(auto& s) {
    using eagine::enumerator_name;
    eagitest::case_ test{s, 3, "enumerator name"};

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
    eagitest::case_ test{s, 4, "from string"};

    test.check(
      eagine::test_enum_1::a ==
        from_string(
          "a", std::type_identity<eagine::test_enum_1>{}, default_selector),
      "1::a");
    test.check(
      eagine::test_enum_1::b ==
        from_string(
          "b", std::type_identity<eagine::test_enum_1>{}, default_selector),
      "1::b");
    test.check(
      eagine::test_enum_1::c ==
        from_string(
          "c", std::type_identity<eagine::test_enum_1>{}, default_selector),
      "1::c");
    test.check(
      eagine::test_enum_1::d ==
        from_string(
          "d", std::type_identity<eagine::test_enum_1>{}, default_selector),
      "1::d");
    test.check(
      eagine::test_enum_1::e ==
        from_string(
          "e", std::type_identity<eagine::test_enum_1>{}, default_selector),
      "1::e");
    test.check(
      eagine::test_enum_1::f ==
        from_string(
          "f", std::type_identity<eagine::test_enum_1>{}, default_selector),
      "1::f");
    test.check(
      eagine::test_enum_1::g ==
        from_string(
          "g", std::type_identity<eagine::test_enum_1>{}, default_selector),
      "1::g");
    test.check(
      eagine::test_enum_1::h ==
        from_string(
          "h", std::type_identity<eagine::test_enum_1>{}, default_selector),
      "1::h");
    test.check(
      eagine::test_enum_1::i ==
        from_string(
          "i", std::type_identity<eagine::test_enum_1>{}, default_selector),
      "1::i");
    test.check(
      eagine::test_enum_1::j ==
        from_string(
          "j", std::type_identity<eagine::test_enum_1>{}, default_selector),
      "1::j");

    test.check(
      eagine::test_enum_2::one ==
        from_string(
          "one", std::type_identity<eagine::test_enum_2>{}, default_selector),
      "2::one");
    test.check(
      eagine::test_enum_2::two ==
        from_string(
          "two", std::type_identity<eagine::test_enum_2>{}, default_selector),
      "2::two");
    test.check(
      eagine::test_enum_2::three ==
        from_string(
          "three", std::type_identity<eagine::test_enum_2>{}, default_selector),
      "2::three");
    test.check(
      eagine::test_enum_2::four ==
        from_string(
          "four", std::type_identity<eagine::test_enum_2>{}, default_selector),
      "2::four");
}
//------------------------------------------------------------------------------
// for each 1
//------------------------------------------------------------------------------
void enum_for_each_1(auto& s) {
    using eagine::default_selector;
    using eagine::from_string;
    eagitest::case_ test{s, 5, "for each 1"};
    eagitest::track trck{test, 10, 1};

    eagine::for_each_enumerator(
      [&test, &trck](const auto& info) {
          test.check(not info.name.empty(), "has name");
          trck.passed_part(1);
      },
      std::type_identity<eagine::test_enum_1>{});
}
//------------------------------------------------------------------------------
// for each 2
//------------------------------------------------------------------------------
void enum_for_each_2(auto& s) {
    using eagine::default_selector;
    using eagine::from_string;
    eagitest::case_ test{s, 6, "for each 2"};
    eagitest::track trck{test, 4, 1};

    eagine::for_each_enumerator(
      [&test, &trck](const auto& info) {
          test.check(not info.name.empty(), "has name");
          trck.passed_part(1);
      },
      std::type_identity<eagine::test_enum_2>{});
}
//------------------------------------------------------------------------------
// for each 3
//------------------------------------------------------------------------------
void enum_for_each_3(auto& s) {
    using eagine::default_selector;
    using eagine::from_string;
    eagitest::case_ test{s, 7, "for each 3"};
    eagitest::track trck{test, 4, 1};

    eagine::for_each_enumerator(
      [&test, &trck](const auto& info) {
          test.check(not info.name.empty(), "has name");
          trck.passed_part(1);
      },
      std::type_identity<eagine::test_enum_3>{});
}
//------------------------------------------------------------------------------
// roundtrip 1
//------------------------------------------------------------------------------
void enum_roundtrip_1(auto& s) {
    using eagine::default_selector;
    using eagine::from_string;
    eagitest::case_ test{s, 8, "roundtrip 1"};
    eagitest::track trck{test, 10, 1};

    const std::type_identity<eagine::test_enum_1> tid;
    for(const auto& info : eagine::enumerator_mapping(tid, default_selector)) {
        test.check(
          info.enumerator == from_string(info.name, tid, default_selector),
          "same value");
        test.check(
          info.name == enumerator_name(info.enumerator, tid, default_selector),
          "same name");
        trck.passed_part(1);
    }
}
//------------------------------------------------------------------------------
// roundtrip 2
//------------------------------------------------------------------------------
void enum_roundtrip_2(auto& s) {
    using eagine::default_selector;
    using eagine::from_string;
    eagitest::case_ test{s, 9, "roundtrip 2"};
    eagitest::track trck{test, 4, 1};

    const std::type_identity<eagine::test_enum_2> tid;
    for(const auto& info : eagine::enumerator_mapping(tid, default_selector)) {
        test.check(
          info.enumerator == from_string(info.name, tid, default_selector),
          "same value");
        test.check(
          info.name == enumerator_name(info.enumerator, tid, default_selector),
          "same name");
        trck.passed_part(1);
    }
}
//------------------------------------------------------------------------------
// roundtrip 3
//------------------------------------------------------------------------------
void enum_roundtrip_3(auto& s) {
    using eagine::default_selector;
    using eagine::from_string;
    eagitest::case_ test{s, 10, "roundtrip 3"};
    eagitest::track trck{test, 4, 1};

    const std::type_identity<eagine::test_enum_3> tid;
    for(const auto& info : eagine::enumerator_mapping(tid, default_selector)) {
        test.check(
          info.enumerator == from_string(info.name, tid, default_selector),
          "same value");
        test.check(
          info.name == enumerator_name(info.enumerator, tid, default_selector),
          "same name");
        trck.passed_part(1);
    }
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "enumerators", 10};
    test.once(enum_is_consecutive);
    test.once(enum_enumerator_count);
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
