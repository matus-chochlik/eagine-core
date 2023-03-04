/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.logging:adapt_filesystem;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.identifier;
import std;

namespace eagine {
//------------------------------------------------------------------------------
struct std_filesystem_path_entry_adapter {
    const identifier name;
    const std::filesystem::path fsp;
    void operator()(auto& backend) const noexcept {
        backend.add_string(name, "FsPath", string_view(fsp.native()));
    }
};
export auto adapt_entry_arg(
  const identifier name,
  const std::filesystem::path& fsp) noexcept {
    return std_filesystem_path_entry_adapter{.name = name, .fsp = fsp};
}
//------------------------------------------------------------------------------
struct std_filesystem_error_entry_adapter {
    const identifier name;
    const std::filesystem::filesystem_error value;
    void operator()(auto& backend) const noexcept {
        backend.add_string(name, "FlSysError", string_view(value.what()));
        backend.add_string(
          "category",
          "ErrorCtgry",
          string_view(value.code().category().name()));
    }
};
export auto adapt_entry_arg(
  const identifier name,
  const std::filesystem::filesystem_error& value) noexcept {
    return std_filesystem_error_entry_adapter{.name = name, .value = value};
}
//------------------------------------------------------------------------------
} // namespace eagine
