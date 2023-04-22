/// @example eagine/optional_reference.cpp
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

    auto print_size{[](optional_reference<const std::string> s) {
        std::cout << s.member(&std::string::size)
                       .and_then([](auto sz) { return std::optional{sz + 1}; })
                       .value_or(0)
                  << std::endl;
    }};

    print_size({});
    const std::string s{"eagine"};
    print_size(s);

    return 0;
}
