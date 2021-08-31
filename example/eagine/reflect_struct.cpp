/// @example eagine/reflect_struct.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/reflect/data_members.hpp>
#include <iostream>

namespace eagine {

struct example_struct {
    bool b{false};
    char c{'\0'};
    float f{0.F};
    int i{0};
    std::string s{};
    unsigned u{0U};
};

// TODO:
// #if EAGINE_CXX_REFLECTION
// template <>
// struct reflect_data_members_of<example_struct> : std::true_type {};
// #else
template <typename Selector>
constexpr auto data_member_mapping(
  const type_identity<example_struct>,
  const Selector) noexcept {
    using S = example_struct;
    return make_data_member_mapping<S, bool, char, float, int, std::string, unsigned>(
      {"b", &S::b},
      {"c", &S::c},
      {"f", &S::f},
      {"i", &S::i},
      {"s", &S::s},
      {"u", &S::u});
}
//#endif

} // namespace eagine

auto main() -> int {
    using namespace eagine;
    using std::get;

    example_struct s{};
    auto m = map_data_members(s);
    std::get<1>(get<0>(m)) = true;
    std::get<1>(get<1>(m)) = 'R';
    std::get<1>(get<2>(m)) = 3.F;

    std::cout << s.b << std::endl;
    std::cout << s.c << std::endl;
    std::cout << s.f << std::endl;

    return 0;
}
