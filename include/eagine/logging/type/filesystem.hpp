/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_LOGGING_TYPE_FILESYSTEM_HPP
#define EAGINE_LOGGING_TYPE_FILESYSTEM_HPP

#include "../entry_arg.hpp"
#include <filesystem>

namespace eagine {
//------------------------------------------------------------------------------
static inline auto adapt_entry_arg(
  const identifier name,
  const std::filesystem::path& fsp) noexcept {
    return [name, fsps{fsp.native()}](auto& backend) {
        backend.add_string(name, EAGINE_ID(FsPath), string_view(fsps));
    };
}
//------------------------------------------------------------------------------
static inline auto adapt_entry_arg(
  const identifier name,
  const std::filesystem::filesystem_error& value) noexcept {
    return [name, value](auto& backend) {
        backend.add_string(
          name, EAGINE_ID(FlSysError), string_view(value.what()));
        backend.add_string(
          EAGINE_ID(category),
          EAGINE_ID(ErrorCtgry),
          string_view(value.code().category().name()));
    };
}
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_LOGGING_TYPE_FILESYSTEM_HPP
