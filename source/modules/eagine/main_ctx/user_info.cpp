/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.main_ctx:user_info;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.valid_if;
import :parent;
import :object;

namespace eagine {

class user_info_impl;

/// @brief Class providing user-related information.
/// @ingroup main_context
export class user_info : public main_ctx_object {
public:
    user_info(main_ctx_parent);

    /// @brief The user's login name.
    [[nodiscard]] auto login_name() noexcept -> valid_if_not_empty<string_view>;

    /// @brief Paths to the user's home directory.
    [[nodiscard]] auto home_dir_path() noexcept
      -> valid_if_not_empty<string_view>;

    /// @brief Paths to the user's configuration directory.
    [[nodiscard]] auto config_dir_path() noexcept
      -> valid_if_not_empty<string_view>;

private:
    shared_holder<user_info_impl> _pimpl;
    auto _impl() noexcept -> optional_reference<user_info_impl>;
};

} // namespace eagine

