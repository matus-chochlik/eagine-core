/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import std;
import eagine.core.types;
import eagine.core.identifier;
import eagine.core.utility;
//------------------------------------------------------------------------------
void scheduler_remove(auto& s) {
    eagitest::case_ test{s, 1, "empty"};
    eagitest::track trck{test, 0, 1};

    eagine::action_scheduler sched;
    test.check(not sched.has_scheduled("test"), "has not test");

    sched.schedule_repeated("test", std::chrono::seconds{1}, [&] {
        test.fail("should not be called");
        return true;
    });

    test.check(sched.has_scheduled("test"), "test scheduled");

    sched.remove("test");

    test.check(not sched.has_scheduled("test"), "test removed");

    eagine::timeout done{std::chrono::seconds{3}};
    while(not done) {
        trck.checkpoint(1);
        sched.update();
    }
}
//------------------------------------------------------------------------------
void scheduler_repeated(auto& s) {
    eagitest::case_ test{s, 2, "repeated"};
    eagitest::track trck{test, 0, 2};

    eagine::action_scheduler sched;
    test.check(not sched.has_scheduled("test"), "has not test");

    int counter{1};
    sched.schedule_repeated("test", std::chrono::milliseconds{100}, [&] {
        trck.checkpoint(1);
        ++counter;
        return true;
    });

    test.check(sched.has_scheduled("test"), "test scheduled");

    eagine::timeout done{std::chrono::seconds{3}};
    while(not done) {
        trck.checkpoint(2);
        sched.update();
    }

    test.check(counter >= 30, "call count");

    sched.remove("test");

    test.check(not sched.has_scheduled("test"), "test removed");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "scheduler", 2};
    test.once(scheduler_remove);
    test.once(scheduler_repeated);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
