/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.build_info:compiler;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.valid_if;

import <tuple>;

namespace eagine {

/// @brief Returns a short name of the target architecture.
/// @ingroup main_ctx
/// @see architecture_name
export auto architecture_tag() noexcept -> valid_if_not_empty<string_view>;

/// @brief Returns a descriptive name of the target architecture.
/// @ingroup main_ctx
/// @see architecture_tag
export auto architecture_name() noexcept -> valid_if_not_empty<string_view>;

/// @brief Returns the name of the compiler used to build the binaries.
/// @ingroup main_ctx
/// @see compiler_version_major
/// @see compiler_version_minor
/// @see compiler_version_patch
export auto compiler_name() noexcept -> valid_if_not_empty<string_view>;

/// @brief Returns the major version number of the compiler.
/// @ingroup main_ctx
/// @see compiler_name
/// @see compiler_version_minor
/// @see compiler_version_patch
export auto compiler_version_major() noexcept -> valid_if_nonnegative<int>;

/// @brief Returns the minor version number of the compiler.
/// @ingroup main_ctx
/// @see compiler_name
/// @see compiler_version_major
/// @see compiler_version_patch
export auto compiler_version_minor() noexcept -> valid_if_nonnegative<int>;

/// @brief Returns the version patch number of the compiler.
/// @ingroup main_ctx
/// @see compiler_name
/// @see compiler_version_major
/// @see compiler_version_minor
export auto compiler_version_patch() noexcept -> valid_if_nonnegative<int>;

export auto compiler_version_tuple() noexcept -> std::tuple<int, int, int> {
    return {
      extract_or(compiler_version_major(), -1),
      extract_or(compiler_version_minor(), -1),
      extract_or(compiler_version_patch(), -1)};
}

} // namespace eagine
