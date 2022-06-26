/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.build_info:git;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.valid_if;

import <tuple>;

namespace eagine {

/// @brief Returns the name of the git branch from which the executable is built.
/// @ingroup main_ctx
/// @see config_git_date
/// @see config_git_hash_id
/// @see config_git_describe
export auto config_git_branch() noexcept -> valid_if_not_empty<string_view>;

/// @brief Returns the hash of the commit from which the executable is built.
/// @ingroup main_ctx
/// @see config_git_date
/// @see config_git_branch
export auto config_git_hash_id() noexcept -> valid_if_not_empty<string_view>;

/// @brief Returns the date of the commit from which the executable is built.
/// @ingroup main_ctx
/// @see config_git_hash_id
export auto config_git_date() noexcept -> valid_if_not_empty<string_view>;

/// @brief Returns the description of the commit from which the executable is built.
/// @ingroup main_ctx
/// @see config_git_date
/// @see config_git_hash_id
/// @see config_git_branch
export auto config_git_describe() noexcept -> valid_if_not_empty<string_view>;

/// @brief Returns the version number as string.
/// @ingroup main_ctx
/// @see config_git_version_major
/// @see config_git_version_minor
/// @see config_git_version_patch
/// @see config_git_version_commit
/// @see config_git_version_tuple
export auto config_git_version() noexcept -> valid_if_not_empty<string_view>;

/// @brief Returns major version number.
/// @ingroup main_ctx
/// @see config_git_version_minor
/// @see config_git_version_patch
/// @see config_git_version_commit
/// @see config_git_version_tuple
export auto config_git_version_major() noexcept -> valid_if_nonnegative<int>;

/// @brief Returns minor version number.
/// @ingroup main_ctx
/// @see config_git_version_major
/// @see config_git_version_patch
/// @see config_git_version_commit
/// @see config_git_version_tuple
export auto config_git_version_minor() noexcept -> valid_if_nonnegative<int>;

/// @brief Returns patch version number.
/// @ingroup main_ctx
/// @see config_git_version_major
/// @see config_git_version_minor
/// @see config_git_version_commit
/// @see config_git_version_tuple
export auto config_git_version_patch() noexcept -> valid_if_nonnegative<int>;

/// @brief Returns commit version number.
/// @ingroup main_ctx
/// @see config_git_version_major
/// @see config_git_version_minor
/// @see config_git_version_patch
/// @see config_git_version_tuple
export auto config_git_version_commit() noexcept -> valid_if_nonnegative<int>;

/// @brief Returns the complete version number as a tuple (major, minor, patch, commit).
/// @ingroup main_ctx
/// @see config_git_version_major
/// @see config_git_version_minor
/// @see config_git_version_patch
/// @see config_git_version_commit
export auto config_git_version_tuple() noexcept
  -> std::tuple<int, int, int, int>;

} // namespace eagine
