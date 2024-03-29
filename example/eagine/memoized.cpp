/// @example eagine/memoized.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import std;

namespace eagine {

auto main(main_ctx& ctx) -> int {
    using N = std::int64_t;
    memoized<N(N)> fib_memo([](N i, auto& memo) -> N {
        if(i < 2) {
            return 1;
        }
        return memo(i, [](N n, auto& f) { return f(n - 2) + f(n - 1); });
    });

    for(N n = 1; n <= 91; ++n) {
        ctx.cio()
          .print("memo", "fib(${n}) = ${f}")
          .arg("n", n)
          .arg("f", fib_memo(n));
    }

    memoized<N(N)> fact_memo([](N i, auto& memo) -> N {
        if(i < 2) {
            return i;
        }
        return memo(i, [](N n, auto& f) { return n * f(n - 1); });
    });

    for(N n = 1; n <= 20; ++n) {
        ctx.cio()
          .print("memo", "${n} != ${f}")
          .arg("n", n)
          .arg("f", fact_memo(n));
    }

    return 0;
}

} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

