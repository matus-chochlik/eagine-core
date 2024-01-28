/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.app:special_args;

import eagine.core.identifier;
import eagine.core.main_ctx;
import eagine.core.resource;
import std;

namespace eagine {
//------------------------------------------------------------------------------
// common special command line arguments
//------------------------------------------------------------------------------
export auto handle_common_special_args(main_ctx& ctx) -> std::optional<int>;
//------------------------------------------------------------------------------
} // namespace eagine
