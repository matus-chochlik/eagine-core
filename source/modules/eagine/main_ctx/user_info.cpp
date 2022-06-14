/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.main_ctx:user_info;

import eagine.core.memory;
import eagine.core.valid_if;
import <memory>;

namespace eagine {

class user_info_impl;

/// @brief Class providing user-related information.
/// @ingroup main_context
export class user_info {
public:
    /// @brief The user's login name.
    auto login_name() noexcept -> valid_if_not_empty<string_view>;

    /// @brief Paths to the user's home directory.
    auto home_dir_path() noexcept -> valid_if_not_empty<string_view>;

    /// @brief Paths to the user's configuration directory.
    auto config_dir_path() noexcept -> valid_if_not_empty<string_view>;

private:
    std::shared_ptr<user_info_impl> _pimpl;
    auto _impl() noexcept -> user_info_impl*;
};

} // namespace eagine

