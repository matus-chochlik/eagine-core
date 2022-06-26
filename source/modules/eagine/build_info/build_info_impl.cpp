/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#if __has_include(<eagine/config/build_info.hpp>)
#include <eagine/config/build_info.hpp>
#endif

#ifndef EAGINE_INSTALL_PREFIX
#define EAGINE_INSTALL_PREFIX ""
#endif

module eagine.core.build_info;

import eagine.core.memory;
import eagine.core.valid_if;

namespace eagine {
//------------------------------------------------------------------------------
auto build_info::install_prefix() const noexcept
  -> valid_if_not_empty<string_view> {
    return {EAGINE_INSTALL_PREFIX};
}
//------------------------------------------------------------------------------
auto build_info::config_dir_path() const noexcept
  -> valid_if_not_empty<string_view> {
    return {EAGINE_INSTALL_PREFIX "/etc"};
}
//------------------------------------------------------------------------------
} // namespace eagine
