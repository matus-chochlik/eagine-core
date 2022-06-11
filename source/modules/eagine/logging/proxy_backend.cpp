/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.logging:proxy_backend;

import :backend;
import <memory>;

namespace eagine {
//------------------------------------------------------------------------------
export auto make_proxy_log_backend(log_stream_info info)
  -> std::unique_ptr<logger_backend>;
//------------------------------------------------------------------------------
} // namespace eagine
