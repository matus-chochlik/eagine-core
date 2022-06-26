/// @example eagine/tribool.cpp
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
#include <eagine/tribool.hpp>
#include <iostream>
#endif

void print(const char* expr, const eagine::weakbool v) {
    std::cout << "'" << expr << "': ";
    if(!v) {
        std::cout << "no" << std::endl;
    } else if(v) {
        std::cout << "yes" << std::endl;
    } else {
        std::cout << "maybe" << std::endl;
    }
}

void print(const char* expr, const eagine::tribool v) {
    std::cout << "'" << expr << "': ";
    if(v) {
        std::cout << "yes" << std::endl;
    } else if(!v) {
        std::cout << "no" << std::endl;
    } else {
        std::cout << "maybe" << std::endl;
    }
}

#define EAG_PRINT(EXPR) print(#EXPR, EXPR)

auto main() -> int {
    using namespace eagine;

    tribool yes{true};
    tribool no{false};
    tribool maybe{indeterminate};

    EAG_PRINT(yes);
    EAG_PRINT(no);
    EAG_PRINT(maybe);

    EAG_PRINT(~yes);
    EAG_PRINT(~no);
    EAG_PRINT(~maybe);

    EAG_PRINT(yes && yes);
    EAG_PRINT(yes && no);
    EAG_PRINT(yes && maybe);
    EAG_PRINT(no && yes);
    EAG_PRINT(no && no);
    EAG_PRINT(no && maybe);
    EAG_PRINT(maybe && maybe);

    EAG_PRINT(yes || yes);
    EAG_PRINT(yes || no);
    EAG_PRINT(yes || maybe);
    EAG_PRINT(no || yes);
    EAG_PRINT(no || no);
    EAG_PRINT(no || maybe);
    EAG_PRINT(maybe || maybe);

    return 0;
}
