/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.logging;
//------------------------------------------------------------------------------
void logger_severity_increased(auto& s) {
    eagitest::case_ test{s, 1, "severity increased"};

    using les = eagine::log_event_severity;

    test.check(increased(les::backtrace) == les::trace, "backtrace");
    test.check(increased(les::trace) == les::debug, "trace");
    test.check(increased(les::debug) == les::stat, "debug");
    test.check(increased(les::stat) == les::info, "stat");
    test.check(increased(les::info) == les::change, "info");
    test.check(increased(les::change) == les::warning, "change");
    test.check(increased(les::warning) == les::error, "warning");
    test.check(increased(les::error) == les::fatal, "error");
    test.check(increased(les::fatal) == les::fatal, "fatal");
}
//------------------------------------------------------------------------------
void logger_severity_decreased(auto& s) {
    eagitest::case_ test{s, 2, "severity decreased"};

    using les = eagine::log_event_severity;

    test.check(decreased(les::backtrace) == les::backtrace, "backtrace");
    test.check(decreased(les::trace) == les::backtrace, "trace");
    test.check(decreased(les::debug) == les::trace, "debug");
    test.check(decreased(les::stat) == les::debug, "stat");
    test.check(decreased(les::info) == les::stat, "info");
    test.check(decreased(les::change) == les::info, "change");
    test.check(decreased(les::warning) == les::change, "warning");
    test.check(decreased(les::error) == les::warning, "error");
    test.check(decreased(les::fatal) == les::error, "fatal");
}
//------------------------------------------------------------------------------
void logger_severity_inc_dec(auto& s) {
    eagitest::case_ test{s, 3, "severity inc/dec"};

    using les = eagine::log_event_severity;

    test.check(increased(decreased(les::backtrace)) == les::trace, "bt");
    test.check(increased(decreased(les::trace)) == les::trace, "trace");
    test.check(increased(decreased(les::debug)) == les::debug, "debug");
    test.check(increased(decreased(les::stat)) == les::stat, "stat");
    test.check(increased(decreased(les::info)) == les::info, "info");
    test.check(increased(decreased(les::change)) == les::change, "change");
    test.check(increased(decreased(les::warning)) == les::warning, "warning");
    test.check(increased(decreased(les::error)) == les::error, "error");
    test.check(increased(decreased(les::fatal)) == les::fatal, "fatal");
}
//------------------------------------------------------------------------------
void logger_severity_dec_inc(auto& s) {
    eagitest::case_ test{s, 4, "severity dec/inc"};

    using les = eagine::log_event_severity;

    test.check(decreased(increased(les::backtrace)) == les::backtrace, "bt");
    test.check(decreased(increased(les::trace)) == les::trace, "trace");
    test.check(decreased(increased(les::debug)) == les::debug, "debug");
    test.check(decreased(increased(les::stat)) == les::stat, "stat");
    test.check(decreased(increased(les::info)) == les::info, "info");
    test.check(decreased(increased(les::change)) == les::change, "change");
    test.check(decreased(increased(les::warning)) == les::warning, "warning");
    test.check(decreased(increased(les::error)) == les::error, "error");
    test.check(decreased(increased(les::fatal)) == les::error, "fatal");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "logging", 4};
    test.once(logger_severity_increased);
    test.once(logger_severity_decreased);
    test.once(logger_severity_inc_dec);
    test.once(logger_severity_dec_inc);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
