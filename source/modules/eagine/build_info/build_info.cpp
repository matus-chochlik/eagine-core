/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.build_info;

export import :git;
export import :version;
export import :compiler;
export import :os;
import eagine.core.memory;
import eagine.core.valid_if;

namespace eagine {

/// @brief Class providing basic system information.
/// @ingroup main_context
export class build_info {
public:
    /// @brief Paths to the install directory.
    /// @see config_dir_path
    [[nodiscard]] auto install_prefix() const noexcept
      -> valid_if_not_empty<string_view>;

    /// @brief Paths to the install prefix configuration directory.
    /// @see install_prefix
    [[nodiscard]] auto config_dir_path() const noexcept
      -> valid_if_not_empty<string_view>;
};

} // namespace eagine
