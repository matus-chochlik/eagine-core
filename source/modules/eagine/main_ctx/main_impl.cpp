/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

module eagine.core.main_ctx;

import <cerrno>;
import std;
import eagine.core.build_info;
import eagine.core.memory;
import eagine.core.identifier;
import eagine.core.valid_if;
import eagine.core.console;
import eagine.core.logging;

namespace eagine {
//------------------------------------------------------------------------------
auto main_impl(
  int argc,
  const char** argv,
  main_ctx_options& options,
  int (*main_func)(main_ctx&)) -> int {
    try {
        main_ctx_storage storage{argc, argv, options};
        main_ctx ctx{storage};
        try {
            assert(main_func);
            return main_func(ctx);
        } catch(const std::system_error& sys_err) {
            ctx.log()
              .error("unhandled system error: ${error}")
              .arg("error", sys_err);
        } catch(const std::exception& err) {
            ctx.log()
              .error("unhandled generic error: ${error}")
              .arg("error", err);
        }
    } catch(const log_backend_error& log_err) {
        const auto code{log_err.code().value()};
        if((code == ENOENT) or (code == ECONNREFUSED)) {
            std::cerr << "Logger failed to connect to the formatter. "
                      << "Start one of the 'xml_logs-*' apps";
            if(const auto prefix{build_info().install_prefix()}) {
                std::cerr << " installed in '" << prefix << "/bin'";
            }
            std::cerr << "." << std::endl;
        } else {
            std::cerr << "unhandled log back-end error: " << log_err.what()
                      << std::endl;
        }
    } catch(const std::system_error& sys_err) {
        std::cerr << "unhandled system error: " << sys_err.what() << std::endl;
    }
    return 1;
}
//------------------------------------------------------------------------------
auto test_main_impl(int argc, const char** argv, int (*main_func)(test_ctx&))
  -> int {
    // TODO: update this when a separate test_ctx implementation is done
    main_ctx_options options{};
    options.logger_opts.forced_backend = make_null_log_backend();
    options.console_opts.forced_backend = make_null_console_backend();
    return main_impl(argc, argv, options, main_func);
}
//------------------------------------------------------------------------------
} // namespace eagine
