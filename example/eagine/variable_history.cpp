/// @example eagine/variable_history.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import std;

namespace eagine {

auto main(main_ctx& ctx) -> int {
    using var_t = eagine::variable_with_history<int, 3>;

    const int n = 20;
    var_t i2(n * n);
    i2.sync();

    for(int i = -n; i <= n; ++i) {
        i2.assign(i * i);
        ctx.cio()
          .print("history", "${old}|${new}|${delta}|${distance}|${delta2}")
          .arg("old", i2.old_value())
          .arg("new", i2.value())
          .arg("delta", i2.delta())
          .arg("distance", i2.distance())
          .arg("delta2", i2.deltas().delta());
    }

    return 0;
}

} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

