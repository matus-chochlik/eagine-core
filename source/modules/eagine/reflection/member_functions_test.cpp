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
//------------------------------------------------------------------------------
struct foobar {
    auto foo() const noexcept -> int {
        return 420;
    }

    auto inv(int x) const noexcept -> float {
        return 1.F / float(x);
    }

    auto add(int l, int r) const noexcept -> int {
        return l + r;
    }

    auto sub(int l, int r) const noexcept -> int {
        return l - r;
    }

    auto mul(int l, int r) const noexcept -> int {
        return l * r;
    }
};
//------------------------------------------------------------------------------
template <identifier_t Id>
constexpr auto member_function_mapping(
  const std::type_identity<foobar>,
  const selector<Id>) noexcept {
    return make_member_function_mapping<
      &foobar::foo,
      &foobar::inv,
      &foobar::add,
      &foobar::sub,
      &foobar::mul>(
      {"foo", {}},
      {"inv", {{"x"}}},
      {"add", {{"l", "r"}}},
      {"sub", {{"l", "r"}}},
      {"mul", {{"l", "r"}}});
}
//------------------------------------------------------------------------------
} // namespace eagine
//------------------------------------------------------------------------------
// member function count
//------------------------------------------------------------------------------
void member_function_count(auto& s) {
    eagitest::case_ test{s, 1, "member function count"};

    using eagine::member_function_count;
    using eagine::member_function_mapping;

    test.check_equal(
      member_function_count(member_function_mapping<eagine::foobar>()),
      5,
      "count 5");
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "member functions", 1};
    test.once(member_function_count);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>

