/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.logging:asio_backend;

import eagine.core.memory;
import :backend;
import <memory>;

namespace eagine {
//------------------------------------------------------------------------------
export auto make_asio_local_ostream_log_backend(const log_stream_info&)
  -> std::unique_ptr<logger_backend>;

export auto make_asio_local_ostream_log_backend(
  string_view addr,
  const log_stream_info&) -> std::unique_ptr<logger_backend>;

export auto make_asio_tcpipv4_ostream_log_backend(
  string_view addr,
  const log_stream_info&) -> std::unique_ptr<logger_backend>;
//------------------------------------------------------------------------------
} // namespace eagine
