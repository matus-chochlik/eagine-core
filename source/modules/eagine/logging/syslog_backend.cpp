/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.logging:syslog_backend;

import eagine.core.types;
import :backend;
import std;

namespace eagine {
//------------------------------------------------------------------------------
export auto make_syslog_log_backend_mutex(const log_stream_info&)
  -> std::unique_ptr<logger_backend>;
export auto make_syslog_log_backend_spinlock(const log_stream_info&)
  -> std::unique_ptr<logger_backend>;
//------------------------------------------------------------------------------
} // namespace eagine

