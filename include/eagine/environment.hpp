/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_ENVIRONMENT_HPP
#define EAGINE_ENVIRONMENT_HPP

#include "c_str.hpp"
#include "valid_if/decl.hpp"
#include <cstdlib>

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Returns the value in the specified environment variable.
/// @ingroup main_context
/// @see application_config
static inline auto get_environment_variable(
  const string_view variable_name) noexcept -> optionally_valid<string_view> {
    if(const auto value{::getenv(c_str(variable_name))}) {
        return {string_view(value), true};
    }
    return {};
}
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_ENVIRONMENT_HPP
