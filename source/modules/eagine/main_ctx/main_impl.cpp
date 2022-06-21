/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.main_ctx;

import eagine.core.memory;
import eagine.core.identifier;
import eagine.core.logging;
import <stdexcept>;
import <system_error>;

namespace eagine {
//------------------------------------------------------------------------------
extern auto main(main_ctx& ctx) -> int;
//------------------------------------------------------------------------------
auto main_impl(int argc, const char** argv, main_ctx_options& options) -> int {
    main_ctx_storage storage{argc, argv, options};
    main_ctx ctx{storage};
    try {
        return eagine::main(ctx);
    } catch(const std::system_error& sys_err) {
        ctx.log()
          .error("unhandled system error: ${error}")
          .arg(identifier{"error"}, sys_err);
    } catch(const std::exception& err) {
        ctx.log()
          .error("unhandled generic error: ${error}")
          .arg(identifier{"error"}, err);
    }
    return 1;
}
//------------------------------------------------------------------------------
} // namespace eagine
