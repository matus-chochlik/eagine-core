/// @example eagine/make_index.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import std;

auto main() -> int {
    using namespace eagine;
    using namespace eagine::memory;

    std::array<const std::string, 4> list{{"foo", "baz", "qux", "bar"}};
    std::vector<std::size_t> index(list.size());

    generate(cover(index), [i{std::size_t{0}}]() mutable { return i++; });

    make_index(view(list), cover(index));

    for(const auto i : index) {
        std::cout << list[i] << std::endl;
    }

    make_index(view(list), cover(index), std::greater<>());

    for(const auto i : index) {
        std::cout << list[i] << std::endl;
    }

    return 0;
}
