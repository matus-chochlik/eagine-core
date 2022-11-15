/// @example eagine/extract.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import <iostream>;
import <memory>;

auto main() -> int {
    using namespace eagine;

    if(const auto optstr{std::make_unique<std::string>("a string")}) {
        std::cout << extract(optstr) << std::endl;
    }

    return 0;
}
