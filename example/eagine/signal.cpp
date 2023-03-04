/// @example eagine/signal.cpp
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

    eagine::signal<void(int, int) noexcept> two_ints;

    const auto add = [](int l, int r) {
        std::cout << l << "+" << r << "=" << l + r << std::endl;
    };

    const auto subtract = [](int l, int r) {
        std::cout << l << "-" << r << "=" << l - r << std::endl;
    };

    two_ints.connect({construct_from, add});
    two_ints.connect({construct_from, subtract});

    two_ints(1, 1);
    two_ints(3, 2);

    return 0;
}
