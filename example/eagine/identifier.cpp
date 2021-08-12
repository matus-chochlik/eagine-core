/// @example eagine/identifier.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/identifier.hpp>
#include <iostream>
//------------------------------------------------------------------------------
static inline void print_info(const eagine::identifier id) {
    std::cout << id.name() << ':' << std::endl;
    std::cout << "  size: " << id.size() << std::endl;
    std::cout << "  max_size: " << id.max_size() << std::endl;
    std::cout << "  sizeof: " << sizeof(id) << std::endl;
    std::cout << "  value: " << id.value() << std::endl;
}
//------------------------------------------------------------------------------
#if EAGINE_HAS_LONG_ID
static inline void print_info(const eagine::long_identifier id) {
    std::cout << id.name() << ':' << std::endl;
    std::cout << "  size: " << id.size() << std::endl;
    std::cout << "  max_size: " << id.max_size() << std::endl;
    std::cout << "  sizeof: " << sizeof(id) << std::endl;
}
#endif
//------------------------------------------------------------------------------
auto main() -> int {
    using namespace eagine;

    print_info(EAGINE_ID(foo));
    print_info(EAGINE_ID(bar));
    print_info(EAGINE_ID(foobarbaz));
    print_info(EAGINE_ID(0123456789));

#if EAGINE_HAS_LONG_ID
    print_info(EAGINE_LONG_ID(ThisIsSomeVeryLongId));
#endif

    return 0;
}
