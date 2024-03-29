/// @example eagine/progress_bar.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import std;

auto main() -> int {
    using namespace eagine;
    signal_switch interrupted;

    float x = 0.F;

    progress_bar pb;
    while(not interrupted) {
        std::cout << pb.update(std::sin(x)).reformat().get() << std::endl;
        x += 0.01F + 0.05F * (std::sin(1.618F * x) + 1.F);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    return 0;
}
