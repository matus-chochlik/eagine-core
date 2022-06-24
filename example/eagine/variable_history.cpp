/// @example eagine/variable_history.cpp
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
#include <eagine/main_ctx.hpp>
#include <eagine/value_with_history.hpp>
#include <iostream>
#endif

namespace eagine {

auto main(main_ctx& ctx) -> int {
    using var_t = eagine::variable_with_history<int, 3>;

    const int n = 20;
    var_t i2(n * n);
    i2.sync();

    for(int i = -n; i <= n; ++i) {
        i2.assign(i * i);
        ctx.cio()
          .print(
            identifier{"history"},
            "${old}|${new}|${delta}|${distance}|${delta2}")
          .arg(identifier{"old"}, i2.old_value())
          .arg(identifier{"new"}, i2.value())
          .arg(identifier{"delta"}, i2.delta())
          .arg(identifier{"distance"}, i2.distance())
          .arg(identifier{"delta2"}, i2.deltas().delta());
    }

    return 0;
}

} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

