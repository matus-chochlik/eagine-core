/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
import std;
import eagine.core;
import eagine.core.log_server;

#include <cassert>

namespace eagine {
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
auto main(main_ctx& ctx) -> int {
    try {
        if(handle_special_args(ctx)) {
            return 0;
        }
        if(const auto sinks{logs::make_sink_factory(ctx)}) {
            logs::internal_backend::set_sink(sinks->make_stream());
            if(const auto reader{logs::make_reader(ctx, sinks)}) {
                if(reader->run()) {
                    return 0;
                }
            }
        }
    } catch(std::exception& error) {
        ctx.log().error("log server error: ${what}").arg("what", error.what());
    }
    return 1;
}
//------------------------------------------------------------------------------
} // namespace eagine
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagine::main_ctx_options options;
    options.app_id = "LogServer";
    options.logger_opts.forced_backend.emplace_derived(
      eagine::hold<eagine::logs::internal_backend>);
    return eagine::main_impl(argc, argv, options, eagine::main);
}
//------------------------------------------------------------------------------

