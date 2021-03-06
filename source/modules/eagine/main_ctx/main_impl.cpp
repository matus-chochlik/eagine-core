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

import eagine.core.memory;
import eagine.core.identifier;
import eagine.core.logging;
import <stdexcept>;
import <system_error>;

namespace eagine {
//------------------------------------------------------------------------------
auto main_impl(
  int argc,
  const char** argv,
  main_ctx_options& options,
  int (*main_func)(main_ctx&)) -> int {
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
        ctx.log().error("unhandled generic error: ${error}").arg("error", err);
    }
    return 1;
}
//------------------------------------------------------------------------------
} // namespace eagine
