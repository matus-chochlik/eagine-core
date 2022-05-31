/// @example eagine/variable_history.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/console/console.hpp>
#include <eagine/main.hpp>
#include <eagine/value_with_history.hpp>
#include <iostream>

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
            EAGINE_ID(history), "${old}|${new}|${delta}|${distance}|${delta2}")
          .arg(EAGINE_ID(old), i2.old_value())
          .arg(EAGINE_ID(new), i2.value())
          .arg(EAGINE_ID(delta), i2.delta())
          .arg(EAGINE_ID(distance), i2.distance())
          .arg(EAGINE_ID(delta2), i2.deltas().delta());
    }

    return 0;
}

} // namespace eagine
