/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.app:special_args;

import eagine.core.identifier;
import eagine.core.main_ctx;
import eagine.core.resource;
import std;

namespace eagine {
//------------------------------------------------------------------------------
export void print_version(main_ctx&, std::ostream&);
//------------------------------------------------------------------------------
export void print_copyright_notice(main_ctx&, std::ostream&);
//------------------------------------------------------------------------------
void do_print_resource(main_ctx&, std::ostream&, const embedded_resource& res);
//------------------------------------------------------------------------------
export void print_bash_completion(main_ctx&, std::ostream&);
//------------------------------------------------------------------------------
// common special command line arguments
//------------------------------------------------------------------------------
export auto handle_common_special_args(main_ctx& ctx) -> std::optional<int>;
//------------------------------------------------------------------------------
} // namespace eagine
