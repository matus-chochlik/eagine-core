/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#if __has_include(<eagine/config/os_info.hpp>)
#include <eagine/config/os_info.hpp>
#endif

module eagine.core.build_info:os;

import eagine.core.memory;
import eagine.core.valid_if;

namespace eagine {
//------------------------------------------------------------------------------
auto config_os_name() noexcept -> valid_if_not_empty<string_view> {
#if defined(EAGINE_CONFIG_OS_INFO_HPP)
    return {string_view{EAGINE_OS_NAME}};
#else
    return {};
#endif
}
//------------------------------------------------------------------------------
auto config_os_code_name() noexcept -> valid_if_not_empty<string_view> {
#if defined(EAGINE_CONFIG_OS_INFO_HPP)
    return {string_view{EAGINE_OS_CODE_NAME}};
#else
    return {};
#endif
}
//------------------------------------------------------------------------------
} // namespace eagine

