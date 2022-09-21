/// @example eagine/embed_self.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import <iostream>;

auto main() -> int {
    using namespace eagine;
    std::cout << hexdump(embed<"self">(__FILE__));
    std::cout << std::endl;
    std::cout << hexdump(search_resource("self"));
    std::cout << std::endl;
    embedded_resource_loader resources;
    std::cout << hexdump(resources.search("self"));

    return 0;
}
