/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.console;

import std;
import eagine.core.types;
import eagine.core.identifier;
import eagine.core.runtime;
import :backend;

namespace eagine {
//------------------------------------------------------------------------------
auto console_init_backend(const program_args&, console_options& opts)
  -> shared_holder<console_backend> {
    if(opts.forced_backend) {
        return std::move(opts.forced_backend);
    }
    // TODO other backends?
    return make_iostream_console_backend(std::cin, std::cout);
}
//------------------------------------------------------------------------------
} // namespace eagine
