/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.runtime:basic_config;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.utility;
import <string>;

namespace eagine {
//------------------------------------------------------------------------------
export struct basic_config : abstract<basic_config> {

    /// @brief Checks is the boolean option identified by @p key is set to true.
    virtual auto is_set(const string_view key, const string_view tag) noexcept
      -> bool = 0;

    /// @brief Checks is the boolean option identified by @p key is set to true.
    auto is_set(const string_view key) noexcept -> bool {
        return is_set(key, {});
    }

    /// @brief Fetches the configuration value identified by @p key, into @p dest.
    virtual auto fetch_string(
      const string_view key,
      const string_view tag,
      std::string& dest) noexcept -> bool = 0;

    /// @brief Fetches the configuration value identified by @p key, into @p dest.
    auto fetch_string(const string_view key, std::string& dest) noexcept
      -> bool {
        return fetch_string(key, {}, dest);
    }
};
//------------------------------------------------------------------------------
} // namespace eagine

