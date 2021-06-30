/// @example eagine/assert.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/assert.hpp>

auto baz(int n) -> int {
    EAGINE_ASSERT(n == 0);
    return n;
}

auto bar(int a, int b, bool c) -> int {
    return baz(c ? a : b);
}

auto foo(int a, int b) -> int {
    return bar(a + b, b - a, true);
}

auto main() -> int {
    return foo(135, 246);
}
