/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.logging:null_backend;

import :backend;
import <memory>;

namespace eagine {
//------------------------------------------------------------------------------
export auto make_null_log_backend() -> std::unique_ptr<logger_backend>;
//------------------------------------------------------------------------------
} // namespace eagine
