/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.runtime:basic_config;
import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.utility;
import eagine.core.reflection;
import eagine.core.valid_if;
import :program_args;

namespace eagine {
//------------------------------------------------------------------------------
export struct basic_config_intf : abstract<basic_config_intf> {

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

    /// @brief Finds and returns configuration string identified by @p key, as optional
    auto find(const string_view key, const std::string_view tag) noexcept
      -> optionally_valid<std::string> {
        std::string temp;
        const bool fetched{fetch_string(key, tag, temp)};
        return {std::move(temp), fetched};
    }

    /// @brief Finds and returns configuration string identified by @p key, as optional
    auto find(const string_view key) noexcept -> optionally_valid<std::string> {
        std::string temp;
        const bool fetched{fetch_string(key, temp)};
        return {std::move(temp), fetched};
    }

    /// @brief Fetches the configuration value identified by @p key, into @p dest.
    template <typename T>
    auto fetch(const string_view key, const string_view tag, T& dest) noexcept
      -> bool {
        return find(key, tag).and_then(assign_if_fits(_1, dest)).or_false();
    }

    /// @brief Fetches the configuration value identified by @p key, into @p dest.
    template <typename T>
    auto fetch(const string_view key, T& dest) noexcept -> bool {
        return find(key).and_then(assign_if_fits(_1, dest)).or_false();
    }
};
//------------------------------------------------------------------------------
export class basic_config : public basic_config_intf {
public:
    basic_config(const program_args& args) noexcept
      : _args{args} {}

    auto program_arg_name(string_view key, const string_view tag) const noexcept
      -> std::string;
    auto environment_var_name(string_view key, const string_view tag)
      const noexcept -> std::string;

    auto find_program_arg(string_view key, const string_view tag) const noexcept
      -> program_arg;

    auto eval_environment_var(string_view key, const string_view tag)
      -> optionally_valid<string_view>;

    using basic_config_intf::is_set;

    auto is_set(const string_view key, const string_view tag) noexcept
      -> bool override;

    using basic_config_intf::fetch_string;

    auto fetch_string(
      const string_view key,
      const string_view tag,
      std::string& dest) noexcept -> bool override;

private:
    program_args _args;
};
//------------------------------------------------------------------------------
} // namespace eagine

