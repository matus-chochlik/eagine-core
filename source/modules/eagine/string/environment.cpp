/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.string:environment;

import eagine.core.memory;
import :c_str;
import <cstdlib>;
import <optional>;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Returns the value in the specified environment variable.
/// @ingroup main_context
/// @see application_config
export auto get_environment_variable(const string_view variable_name) noexcept
  -> std::optional<string_view> {
    if(const auto value{::getenv(c_str(variable_name))}) {
        return {string_view(value)};
    }
    return {};
}
//------------------------------------------------------------------------------
} // namespace eagine
