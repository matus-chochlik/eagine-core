/// @example eagine/embed_self.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import std;

auto main() -> int {
    using namespace eagine;
    std::cout << hexdump(embed<"self">(__FILE__));
    std::cout << std::endl;
    std::cout << hexdump(search_resource("self"));
    std::cout << std::endl;
    embedded_resource_loader resources;
    std::cout << hexdump(resources.search("self"));

    resources.for_each([](const auto res_id, const auto& res) {
        std::cout << identifier{res_id}.name() << "|"
                  << res.embedded_block().size() << std::endl;
    });

    return 0;
}
