/// @example eagine/network_sorter.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import <iostream>;
import <functional>;
import <random>;
import <vector>;

auto main() -> int {
    using namespace eagine;
    using namespace eagine::memory;

    std::vector<int> vec(32);

    do {
        shuffle(
          generate(cover(vec), [i{0}]() mutable { return i++; }),
          std::random_device());
    } while(is_sorted(view(vec), std::less<>()));

    std::cout << "shuffled: " << view(vec) << std::endl;
    std::cout << "ascending: " << network_sort<32, std::less<>>(cover(vec))
              << std::endl;
    std::cout << "decending: " << network_sort<32, std::greater<>>(cover(vec))
              << std::endl;

    return 0;
}
