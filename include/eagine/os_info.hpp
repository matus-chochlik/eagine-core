/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_OS_INFO_HPP
#define EAGINE_OS_INFO_HPP

#include "string_span.hpp"
#include "valid_if/nonnegative.hpp"
#include "valid_if/not_empty.hpp"
#include <tuple>

namespace eagine {

/// @brief Returns the name of the OS on which the executable is built.
/// @ingroup main_ctx
/// @see config_os_code_name
auto config_os_name() noexcept -> valid_if_not_empty<string_view>;

/// @brief Returns the code-name of the OS on which the executable is built.
/// @ingroup main_ctx
/// @see config_os_name
auto config_os_code_name() noexcept -> valid_if_not_empty<string_view>;

} // namespace eagine

#if !EAGINE_CORE_LIBRARY || defined(EAGINE_IMPLEMENTING_CORE_LIBRARY)
#include <eagine/os_info.inl>
#endif

#endif // EAGINE_OS_INFO_HPP
