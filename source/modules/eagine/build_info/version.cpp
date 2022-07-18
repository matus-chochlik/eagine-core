/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.build_info:version;

import eagine.core.types;
import eagine.core.valid_if;
import :git;
import :compiler;

import <tuple>;
import <type_traits>;

namespace eagine {

/// @brief Class providing basic system information.
/// @ingroup main_context
export class version_info
  : public structural_core<std::tuple<int, int, int, int>> {

    using base = structural_core<std::tuple<int, int, int, int>>;

public:
    version_info() noexcept = default;

    constexpr version_info(std::tuple<int, int, int, int> data) noexcept
      : base{std::move(data)} {}

    constexpr version_info(std::tuple<int, int, int> data) noexcept
      : base{{std::get<0>(data), std::get<1>(data), std::get<2>(data), -1}} {}

    /// @brief Creates an instance of git source version info.
    /// @see compiler
    static auto git() noexcept -> version_info {
        return {config_git_version_tuple()};
    }

    /// @brief Creates an instance of git source version info.
    /// @see git
    static auto compiler() noexcept -> version_info {
        return {compiler_version_tuple()};
    }

    /// @brief Returns the project version numbers in a single tuple.
    /// @see version_major
    /// @see version_minor
    /// @see version_patch
    /// @see version_commit
    auto version_tuple() const noexcept
      -> const std::tuple<int, int, int, int>& {
        return get_structure();
    }

    /// @brief Returns the project major version number.
    /// @see version_tuple
    /// @see version_minor
    /// @see version_patch
    /// @see version_commit
    auto version_major() const noexcept -> valid_if_nonnegative<int> {
        return {std::get<0>(version_tuple())};
    }

    /// @brief Returns the project minor version number.
    /// @see version_tuple
    /// @see version_major
    /// @see version_patch
    /// @see version_commit
    auto version_minor() const noexcept -> valid_if_nonnegative<int> {
        return {std::get<1>(version_tuple())};
    }

    /// @brief Returns the project patch number.
    /// @see version_tuple
    /// @see version_major
    /// @see version_minor
    /// @see version_commit
    auto version_patch() const noexcept -> valid_if_nonnegative<int> {
        return {std::get<2>(version_tuple())};
    }

    /// @brief Returns the project commit number.
    /// @see version_tuple
    /// @see version_major
    /// @see version_minor
    /// @see version_patch
    auto version_commit() const noexcept -> valid_if_nonnegative<int> {
        return {std::get<3>(version_tuple())};
    }

    /// @brief Indicates if major and minor version numbers are known.
    /// @see version_major
    /// @see version_minor
    auto has_version() const noexcept -> bool {
        return version_major() && version_minor();
    }

    /// @brief Checks if the project build version is at least as specified.
    auto version_at_least(int major, int minor) const -> tribool;

    /// @brief Checks if the project build version is at least as specified.
    auto version_at_least(int major, int minor, int patch) const -> tribool;

    /// @brief Checks if the project build version is at least as specified.
    auto version_at_least(int major, int minor, int patch, int commit) const
      -> tribool;
};
//------------------------------------------------------------------------------
auto version_info::version_at_least(int major, int minor) const -> tribool {
    if(const auto opt_maj{version_major()}) {
        if(extract(opt_maj) < major) {
            return false;
        }
        if(extract(opt_maj) > major) {
            return true;
        }
        if(const auto opt_min{version_minor()}) {
            if(extract(opt_min) < minor) {
                return false;
            }
            return true;
        }
    }
    return indeterminate;
}
//------------------------------------------------------------------------------
auto version_info::version_at_least(int major, int minor, int patch) const
  -> tribool {
    if(const auto opt_maj{version_major()}) {
        if(extract(opt_maj) < major) {
            return false;
        }
        if(extract(opt_maj) > major) {
            return true;
        }
        if(const auto opt_min{version_minor()}) {
            if(extract(opt_min) < minor) {
                return false;
            }
            if(extract(opt_min) > minor) {
                return true;
            }
            if(const auto opt_ptch{version_patch()}) {
                if(extract(opt_ptch) < patch) {
                    return false;
                }
                return true;
            }
        }
    }
    return indeterminate;
}
//------------------------------------------------------------------------------
auto version_info::version_at_least(int major, int minor, int patch, int commit)
  const -> tribool {
    if(const auto opt_maj{version_major()}) {
        if(extract(opt_maj) < major) {
            return false;
        }
        if(extract(opt_maj) > major) {
            return true;
        }
        if(const auto opt_min{version_minor()}) {
            if(extract(opt_min) < minor) {
                return false;
            }
            if(extract(opt_min) > minor) {
                return true;
            }
            if(const auto opt_ptch{version_patch()}) {
                if(extract(opt_ptch) < patch) {
                    return false;
                }
                if(extract(opt_ptch) > patch) {
                    return true;
                }
                if(const auto opt_cmit{version_commit()}) {
                    if(extract(opt_cmit) < commit) {
                        return false;
                    }
                    return true;
                }
            }
        }
    }
    return indeterminate;
}
//------------------------------------------------------------------------------
} // namespace eagine
