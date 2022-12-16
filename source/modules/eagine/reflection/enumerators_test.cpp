/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
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
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "enumerators", 1};
    test.once(enum_is_consecutive);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
