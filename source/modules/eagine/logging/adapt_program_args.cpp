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
export auto adapt_entry_arg(
  const identifier name,
  const program_arg& value) noexcept {
    return [name, value](auto& backend) {
        backend.add_string(name, identifier{"ProgramArg"}, string_view(value));
    };
}
//------------------------------------------------------------------------------
} // namespace eagine

