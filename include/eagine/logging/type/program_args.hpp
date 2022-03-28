/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_LOGGING_TYPE_PROGRAM_ARGS_HPP
#define EAGINE_LOGGING_TYPE_PROGRAM_ARGS_HPP

#include "../../program_args.hpp"
#include "../entry_arg.hpp"

namespace eagine {
//------------------------------------------------------------------------------
static inline auto adapt_entry_arg(
  const identifier name,
  const program_arg& value) noexcept {
    return [name, value](auto& backend) {
        backend.add_string(name, EAGINE_ID(ProgramArg), string_view(value));
    };
}
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_LOGGING_TYPE_PROGRAM_ARGS_HPP
