/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.logging:adapt_program_args;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.identifier;
import eagine.core.runtime;

namespace eagine {
//------------------------------------------------------------------------------
struct program_arg_entry_adapter {
    const identifier name;
    const program_arg& value;

    void operator()(auto& backend) const noexcept {
        backend.add_string(name, "ProgramArg", string_view(value));
    }
};
export auto adapt_entry_arg(
  const identifier name,
  const program_arg& value) noexcept {
    return program_arg_entry_adapter{.name = name, .value = value};
}
//------------------------------------------------------------------------------
} // namespace eagine

