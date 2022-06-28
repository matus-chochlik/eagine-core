/// @example eagine/dyn_lib_lookup.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#if EAGINE_CORE_MODULE
import eagine.core;
#else
#include <eagine/console/console.hpp>
#include <eagine/dynamic_library.hpp>
#include <eagine/logging/logger.hpp>
#include <eagine/main_ctx.hpp>
#endif

namespace eagine {

auto main(main_ctx& ctx) -> int {

    if(const string_view lib_path{ctx.args().find("--library").next()}) {
        if(shared_executable_module module{lib_path}) {
            if(string_view sym_name{ctx.args().find("--symbol").next()}) {
                if(module.exports(sym_name)) {
                    ctx.cio()
                      .print(identifier{"DynLib"}, "symbol found")
                      .arg(identifier{"library"}, lib_path)
                      .arg(identifier{"symbol"}, sym_name);
                } else {
                    ctx.cio()
                      .print(identifier{"DynLib"}, "symbol not found")
                      .arg(identifier{"library"}, lib_path)
                      .arg(identifier{"symbol"}, sym_name);
                }
            } else {
                ctx.log().error("missing symbol name argument");
            }
        } else {
            ctx.log()
              .error("failed to open ${lib}: ${message}")
              .arg(identifier{"lib"}, lib_path)
              .arg(identifier{"message"}, module.error_message());
        }
    } else {
        ctx.log().error("missing library path argument");
    }

    return 0;
}

} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

