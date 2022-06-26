/// @example eagine/valgrind.cpp
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
#include <eagine/console/console.hpp>
#include <eagine/interop/valgrind.hpp>
#include <eagine/main_ctx.hpp>
#include <iostream>
#endif

namespace eagine {

auto main(main_ctx& ctx) -> int {
    using namespace eagine;

    const identifier vgid{"valgrind"};

    if(const auto on_vg{running_on_valgrind()}) {
        ctx.cio().print(vgid, "running on valgrind");
    } else if(!on_vg) {
        ctx.cio().print(vgid, "not running on valgrind");
    } else {
        ctx.cio().print(vgid, "maybe running on valgrind");
    }

    return 0;
}

} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}
