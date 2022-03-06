/// @example eagine/valid_if.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/valid_if/positive.hpp>
#include <iostream>

auto main() -> int {
    int i = 0;
    eagine::valid_if_positive<int&> vi{i};
    if(has_value(vi)) {
        std::cout << "A" << std::endl;
    }
    i = 1;
    if(has_value(vi)) {
        std::cout << "B" << std::endl;
    }
    vi = 0;
    if(has_value(vi)) {
        std::cout << "C" << std::endl;
    }

    return 0;
}

