/// @example eagine/tribool.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import <iostream>;

void print(const char* expr, const eagine::weakbool v) {
    std::cout << "'" << expr << "': ";
    if(not v) {
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
    } else if(not v) {
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

    EAG_PRINT(yes and yes);
    EAG_PRINT(yes and no);
    EAG_PRINT(yes and maybe);
    EAG_PRINT(no and yes);
    EAG_PRINT(no and no);
    EAG_PRINT(no and maybe);
    EAG_PRINT(maybe and maybe);

    EAG_PRINT(yes or yes);
    EAG_PRINT(yes or no);
    EAG_PRINT(yes or maybe);
    EAG_PRINT(no or yes);
    EAG_PRINT(no or no);
    EAG_PRINT(no or maybe);
    EAG_PRINT(maybe or maybe);

    return 0;
}
