/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.logging:adapt_string_path;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.identifier;

namespace eagine {
//------------------------------------------------------------------------------
struct string_path_entry_adapter {
    const identifier name;
    const basic_string_path value;

    void operator()(auto& backend) const noexcept {
        backend.add_string(name, "StringPath", value.as_string());
    }
};
//------------------------------------------------------------------------------
export auto adapt_entry_arg(
  const identifier name,
  const basic_string_path& value) noexcept {
    return string_path_entry_adapter{.name = name, .value = value};
}
//------------------------------------------------------------------------------
} // namespace eagine

