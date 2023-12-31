/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.log_server;

import std;
import eagine.core;

namespace eagine::logs {
//------------------------------------------------------------------------------
void print_bash_completion(
  main_ctx& ctx,
  std::ostream& out,
  const embedded_resource& res) {
    if(res) {
        const auto print{[&](const memory::const_block data) {
            write_to_stream(out, data);
            return true;
        }};
        res.fetch(ctx, {construct_from, print});
    }
}
//------------------------------------------------------------------------------
auto handle_special_args(main_ctx& ctx) -> bool {
    if(ctx.args().find("--print-bash-completion")) {
        print_bash_completion(ctx, std::cout, search_resource("bashCmpltn"));
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
} // namespace eagine::logs
