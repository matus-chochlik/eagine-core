/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.console;

import eagine.core.identifier;
import eagine.core.runtime;
import <iostream>;
import <utility>;

namespace eagine {
//------------------------------------------------------------------------------
auto console_init_backend(const program_args&, console_options& opts)
  -> std::unique_ptr<console_backend> {
    if(opts.forced_backend) {
        return std::move(opts.forced_backend);
    }
    // TODO other backends?
    return make_iostream_console_backend(std::cin, std::cout);
}
//------------------------------------------------------------------------------
console::console(
  identifier app_id,
  const program_args& args,
  console_options& opts) noexcept
  : _backend{console_init_backend(args, opts)}
  , _app_id{app_id} {}
//------------------------------------------------------------------------------
} // namespace eagine
