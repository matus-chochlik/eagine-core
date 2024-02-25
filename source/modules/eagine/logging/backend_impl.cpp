/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.logging;

import std;

namespace eagine {
//------------------------------------------------------------------------------
auto increased(log_event_severity level) noexcept -> log_event_severity {
    switch(level) {
        case log_event_severity::backtrace:
            return log_event_severity::trace;
        case log_event_severity::trace:
            return log_event_severity::debug;
        case log_event_severity::debug:
            return log_event_severity::stat;
        case log_event_severity::stat:
            return log_event_severity::info;
        case log_event_severity::info:
            return log_event_severity::change;
        case log_event_severity::change:
            return log_event_severity::warning;
        case log_event_severity::warning:
            return log_event_severity::error;
        case log_event_severity::error:
            return log_event_severity::fatal;
        case log_event_severity::fatal:
            return log_event_severity::fatal;
    }
}
//------------------------------------------------------------------------------
auto decreased(log_event_severity level) noexcept -> log_event_severity {
    switch(level) {
        case log_event_severity::backtrace:
            return log_event_severity::backtrace;
        case log_event_severity::trace:
            return log_event_severity::backtrace;
        case log_event_severity::debug:
            return log_event_severity::trace;
        case log_event_severity::stat:
            return log_event_severity::debug;
        case log_event_severity::info:
            return log_event_severity::stat;
        case log_event_severity::change:
            return log_event_severity::info;
        case log_event_severity::warning:
            return log_event_severity::change;
        case log_event_severity::error:
            return log_event_severity::warning;
        case log_event_severity::fatal:
            return log_event_severity::error;
    }
}
//------------------------------------------------------------------------------
} // namespace eagine
