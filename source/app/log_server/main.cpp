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
auto main(main_ctx& ctx) -> int {
    try {
        if(auto sinks{logs::make_sink_factory(ctx)}) {
            logs::internal_backend::set_sink(sinks->make_stream());
            if(auto reader{logs::make_reader(ctx, sinks)}) {
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

