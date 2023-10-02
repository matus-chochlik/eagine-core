/// @example eagine/identifier.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import std;
//------------------------------------------------------------------------------
static inline void print_info(const eagine::identifier id) {
    std::cout << id.name() << ':' << std::endl;
    std::cout << "  size: " << id.size() << std::endl;
    std::cout << "  max_size: " << id.max_size() << std::endl;
    std::cout << "  sizeof: " << sizeof(id) << std::endl;
    std::cout << "  value: " << id.value() << std::endl;
}
//------------------------------------------------------------------------------
auto main() -> int {
    using namespace eagine;

    print_info(identifier{"foo"});
    print_info(identifier{"bar"});
    print_info(identifier{"foobarbaz"});
    print_info(identifier{"0123456789"});

    print_info("baz");
    print_info("FooBarBaz");
    print_info("1234567890");

    identifier id;
    for(int i = 0; i < 1000; ++i) {
        id = increment(id);
        std::cout << id.name() << std::endl;
    }

    return 0;
}
