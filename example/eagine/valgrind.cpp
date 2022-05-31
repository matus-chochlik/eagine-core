/// @example eagine/valgrind.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/console/console.hpp>
#include <eagine/interop/valgrind.hpp>
#include <eagine/main.hpp>
#include <iostream>

namespace eagine {

auto main(main_ctx& ctx) -> int {
    using namespace eagine;

    if(const auto on_vg{running_on_valgrind()}) {
        ctx.cio().print(EAGINE_ID(valgrind), "running on valgrind");
    } else if(!on_vg) {
        ctx.cio().print(EAGINE_ID(valgrind), "not running on valgrind");
    } else {
        ctx.cio().print(EAGINE_ID(valgrind), "maybe running on valgrind");
    }

    return 0;
}

} // namespace eagine
