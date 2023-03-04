/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.console:iostream_backend;

import :backend;
import std;

namespace eagine {
//------------------------------------------------------------------------------
export auto make_iostream_console_backend(std::istream&, std::ostream&)
  -> std::unique_ptr<console_backend>;
//------------------------------------------------------------------------------
} // namespace eagine
