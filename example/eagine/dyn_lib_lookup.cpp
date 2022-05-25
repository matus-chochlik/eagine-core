/// @example eagine/dyn_lib_lookup.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/console/console.hpp>
#include <eagine/dynamic_library.hpp>
#include <eagine/logging/logger.hpp>
#include <eagine/main.hpp>

namespace eagine {

auto main(main_ctx& ctx) -> int {

    if(const string_view lib_path{ctx.args().find("--library").next()}) {
        if(shared_executable_module module{lib_path}) {
            if(string_view sym_name{ctx.args().find("--symbol").next()}) {
                if(module.exports(sym_name)) {
                    ctx.cio()
                      .print(EAGINE_ID(DynLib), "symbol found")
                      .arg(EAGINE_ID(library), lib_path)
                      .arg(EAGINE_ID(symbol), sym_name);
                } else {
                    ctx.cio()
                      .print(EAGINE_ID(DynLib), "symbol not found")
                      .arg(EAGINE_ID(library), lib_path)
                      .arg(EAGINE_ID(symbol), sym_name);
                }
            } else {
                ctx.log().error("missing symbol name argument");
            }
        } else {
            ctx.log()
              .error("failed to open ${lib}: ${message}")
              .arg(EAGINE_ID(lib), lib_path)
              .arg(EAGINE_ID(message), module.error_message());
        }
    } else {
        ctx.log().error("missing library path argument");
    }

    return 0;
}

} // namespace eagine
