/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
import std;
import eagine.core;
import eagine.core.log_server;

#include <cassert>

namespace eagine {
//------------------------------------------------------------------------------
// print text from resource
//------------------------------------------------------------------------------
void do_print_resource(
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
template <std::size_t L>
void print_resource(main_ctx& ctx, std::ostream& out, const char (&res_id)[L]) {
    do_print_resource(ctx, out, search_resource(res_id));
}
//------------------------------------------------------------------------------
// special args handling
//------------------------------------------------------------------------------
auto handle_special_args(main_ctx& ctx) -> std::optional<int> {
    if(ctx.args().find("--print-psql-schema")) {
        print_resource(ctx, std::cout, "PSQLSchema");
        return {0};
    }
    return handle_common_special_args(ctx);
}
//------------------------------------------------------------------------------
// entry point
//------------------------------------------------------------------------------
auto main(main_ctx& ctx) -> int {
    try {
        if(const auto exit_code{handle_special_args(ctx)}) {
            return *exit_code;
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

