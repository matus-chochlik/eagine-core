/// @example eagine/reflect_struct.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import std;

namespace eagine {

struct example_struct {
    bool b{false};
    char c{'\0'};
    float f{0.F};
    int i{0};
    std::string s{};
    unsigned u{0U};
};

template <>
struct data_member_traits<example_struct> {
    static constexpr auto mapping() noexcept {
        using S = example_struct;
        return make_data_member_mapping<
          S,
          bool,
          char,
          float,
          int,
          std::string,
          unsigned>(
          {"b", &S::b},
          {"c", &S::c},
          {"f", &S::f},
          {"i", &S::i},
          {"s", &S::s},
          {"u", &S::u});
    }
};

auto main(main_ctx& ctx) -> int {
    using namespace eagine;
    using std::get;

    example_struct s{};
    auto m = map_data_members(s);
    std::get<1>(get<0>(m)) = true;
    std::get<1>(get<1>(m)) = 'R';
    std::get<1>(get<2>(m)) = 3.F;

    ctx.cio().print("struct", "b: ${b}").arg("b", s.b);
    ctx.cio().print("struct", "c: ${c}").arg("c", s.c);
    ctx.cio().print("struct", "f: ${f}").arg("f", s.f);

    traverse_mapped(s, [&](const auto& path, const auto& value) {
        ctx.cio()
          .print("traverse", "${path}: ${value}")
          .arg("path", path)
          .arg("value", value);
    });

    return 0;
}
} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

