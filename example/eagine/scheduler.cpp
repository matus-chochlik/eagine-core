/// @example eagine/scheduler.cpp
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
    std::chrono::steady_clock clock;

    const auto elapsed{[&, start{clock.now()}] {
        return std::chrono::duration<float>(clock.now() - start);
    }};

    using std::chrono::seconds;

    const auto schedule{[&](identifier id, seconds interval) {
        ctx.scheduler().schedule_repeated(id, interval, [&, id] {
            std::cout << id.name() << ": " << elapsed() << std::endl;
        });
    }};

    schedule("one", seconds{1});
    schedule("two", seconds{2});
    schedule("three", seconds{3});
    schedule("four", seconds{4});
    schedule("five", seconds{5});

    timeout done{std::chrono::seconds{30}};
    while(not done) {
        ctx.update();
        std::this_thread::yield();
    }

    return 0;
}
} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

