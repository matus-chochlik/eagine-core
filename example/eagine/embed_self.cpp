/// @example eagine/embed_self.cpp
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
#include <eagine/embed.hpp>
#include <eagine/hexdump.hpp>
#include <iostream>
#endif

auto main() -> int {
    using namespace eagine;
    std::cout << hexdump(embed<id_v("self")>(__FILE__));

    return 0;
}
