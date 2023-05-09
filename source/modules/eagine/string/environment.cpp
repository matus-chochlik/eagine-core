/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.string:environment;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.valid_if;
import :c_str;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Returns the value in the specified environment variable.
/// @ingroup main_context
/// @see application_config
export [[nodiscard]] auto get_environment_variable(
  const string_view variable_name) noexcept -> optionally_valid<string_view> {
    if(const auto value{::getenv(c_str(variable_name))}) {
        return {string_view(value), true};
    }
    return {};
}
//------------------------------------------------------------------------------
export template <typename X>
[[nodiscard]] constexpr auto get_environment_variable(
  placeholder_expression<X> e) noexcept {
    return placeholder_expression{[e](auto&&... args) {
        return get_environment_variable(
          string_view{e(decltype(args)(args)...)});
    }};
}
//------------------------------------------------------------------------------
} // namespace eagine
