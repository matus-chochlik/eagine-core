/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include "interfaces.hpp"
#include "internal_backend.hpp"

namespace eagine {
//------------------------------------------------------------------------------
auto main(main_ctx& ctx) -> int {
    auto reader{logs::make_reader(ctx, logs::make_sink_factory(ctx))};
    return reader->run() ? 0 : 1;
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

