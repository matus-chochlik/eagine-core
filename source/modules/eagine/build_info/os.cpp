/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.build_info:os;

import eagine.core.memory;
import eagine.core.valid_if;

namespace eagine {

/// @brief Returns the name of the OS on which the executable is built.
/// @ingroup main_ctx
/// @see config_os_code_name
export [[nodiscard]] auto config_os_name() noexcept
  -> valid_if_not_empty<string_view>;

/// @brief Returns the code-name of the OS on which the executable is built.
/// @ingroup main_ctx
/// @see config_os_name
export [[nodiscard]] auto config_os_code_name() noexcept
  -> valid_if_not_empty<string_view>;

} // namespace eagine
