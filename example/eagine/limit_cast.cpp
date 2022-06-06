/// @example eagine/limit_cast.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#if EAGINE_CORE_MODULE
import eagine.core;
import <iostream>;
#else
#include <eagine/is_within_limits.hpp>
#include <iostream>
#endif

static inline void foo(short s) {
    std::cout << s << std::endl;
}

auto main() -> int {
    using namespace eagine;

    // would assert if 123 didn't fit into short
    foo(limit_cast<short>(123));

    if(const auto converted{convert_if_fits<short>(12345)}) {
        std::cout << extract(converted) << std::endl;
    }

    if(const auto converted{convert_if_fits<short>(123456)}) {
        std::cout << extract(converted) << std::endl;
    } else {
        std::cout << "value too big" << std::endl;
    }

    return 0;
}
