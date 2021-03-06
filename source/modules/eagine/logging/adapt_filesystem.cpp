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
import <filesystem>;
import <system_error>;
import <string>;

namespace eagine {
//------------------------------------------------------------------------------
export auto adapt_entry_arg(
  const identifier name,
  const std::filesystem::path& fsp) noexcept {
    return [name, fsps{fsp.native()}](auto& backend) {
        backend.add_string(name, "FsPath", string_view(fsps));
    };
}
//------------------------------------------------------------------------------
export auto adapt_entry_arg(
  const identifier name,
  const std::filesystem::filesystem_error& value) noexcept {
    return [name, value](auto& backend) {
        backend.add_string(name, "FlSysError", string_view(value.what()));
        backend.add_string(
          "category",
          "ErrorCtgry",
          string_view(value.code().category().name()));
    };
}
//------------------------------------------------------------------------------
} // namespace eagine
