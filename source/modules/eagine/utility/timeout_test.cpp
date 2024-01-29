/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import std;
import eagine.core.types;
import eagine.core.utility;
//------------------------------------------------------------------------------
void timeout_1(unsigned, auto& s) {
    eagitest::case_ test{s, 1, "1"};

    using R = std::chrono::milliseconds::rep;
    const auto time_dur{
      std::chrono::milliseconds{test.random().get_between(R(250), R(1500))}};
    const auto start{std::chrono::steady_clock::now()};
    eagine::timeout to{time_dur};
    while(not to.is_expired()) {
        std::this_thread::yield();
    }
    const auto dur{std::chrono::steady_clock::now() - start};
    test.check(dur >= time_dur, "duration at least");
    test.check(dur < time_dur * 2, "duration at most");
}
//------------------------------------------------------------------------------
void timeout_reset(auto& s) {
    eagitest::case_ test{s, 2, "reset"};

    using R = std::chrono::milliseconds::rep;
    eagine::timeout to;
    for(unsigned r = 0; r < test.repeats(10); ++r) {
        const auto time_dur{std::chrono::milliseconds{
          test.random().get_between(R(250), R(1500))}};
        const auto start{std::chrono::steady_clock::now()};
        to.reset(time_dur);
        while(not to.is_expired()) {
            std::this_thread::yield();
        }
        const auto dur{std::chrono::steady_clock::now() - start};
        test.check(dur >= time_dur, "duration at least");
        test.check(dur < time_dur * 2, "duration at most");
    }
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "timeout", 2};
    test.repeat(10, timeout_1);
    test.once(timeout_reset);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
