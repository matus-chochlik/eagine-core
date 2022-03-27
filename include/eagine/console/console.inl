/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/console/iostream_backend.hpp>
#include <iostream>

namespace eagine {
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto console::_init_backend(const program_args&, console_options& opts)
  -> std::unique_ptr<console_backend> {
    if(opts.forced_backend) {
        return std::move(opts.forced_backend);
    }
    // TODO other backends?
    return std::make_unique<iostream_console_backend<>>(std::cout);
}
//------------------------------------------------------------------------------
} // namespace eagine
