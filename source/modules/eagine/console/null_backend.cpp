/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.console:null_backend;

import :backend;
import std;

namespace eagine {
//------------------------------------------------------------------------------
export auto make_null_console_backend() -> std::unique_ptr<console_backend>;
//------------------------------------------------------------------------------
} // namespace eagine
