/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.build_info;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.valid_if;

namespace eagine {
//------------------------------------------------------------------------------
// git
//------------------------------------------------------------------------------
/// @brief Returns the name of the git branch from which the executable is built.
/// @ingroup main_ctx
/// @see config_git_date
/// @see config_git_hash_id
/// @see config_git_describe
export [[nodiscard]] auto config_git_branch() noexcept
  -> valid_if_not_empty<string_view>;

/// @brief Returns the hash of the commit from which the executable is built.
/// @ingroup main_ctx
/// @see config_git_date
/// @see config_git_branch
export [[nodiscard]] auto config_git_hash_id() noexcept
  -> valid_if_not_empty<string_view>;

/// @brief Returns the date of the commit from which the executable is built.
/// @ingroup main_ctx
/// @see config_git_hash_id
export [[nodiscard]] auto config_git_date() noexcept
  -> valid_if_not_empty<string_view>;

/// @brief Returns the description of the commit from which the executable is built.
/// @ingroup main_ctx
/// @see config_git_date
/// @see config_git_hash_id
/// @see config_git_branch
export [[nodiscard]] auto config_git_describe() noexcept
  -> valid_if_not_empty<string_view>;

/// @brief Returns the version number as string.
/// @ingroup main_ctx
/// @see config_git_version_major
/// @see config_git_version_minor
/// @see config_git_version_patch
/// @see config_git_version_commit
/// @see config_git_version_tuple
export [[nodiscard]] auto config_git_version() noexcept
  -> valid_if_not_empty<string_view>;

/// @brief Returns major version number.
/// @ingroup main_ctx
/// @see config_git_version_minor
/// @see config_git_version_patch
/// @see config_git_version_commit
/// @see config_git_version_tuple
export [[nodiscard]] auto config_git_version_major() noexcept
  -> valid_if_nonnegative<int>;

/// @brief Returns minor version number.
/// @ingroup main_ctx
/// @see config_git_version_major
/// @see config_git_version_patch
/// @see config_git_version_commit
/// @see config_git_version_tuple
export [[nodiscard]] auto config_git_version_minor() noexcept
  -> valid_if_nonnegative<int>;

/// @brief Returns patch version number.
/// @ingroup main_ctx
/// @see config_git_version_major
/// @see config_git_version_minor
/// @see config_git_version_commit
/// @see config_git_version_tuple
export [[nodiscard]] auto config_git_version_patch() noexcept
  -> valid_if_nonnegative<int>;

/// @brief Returns commit version number.
/// @ingroup main_ctx
/// @see config_git_version_major
/// @see config_git_version_minor
/// @see config_git_version_patch
/// @see config_git_version_tuple
export [[nodiscard]] auto config_git_version_commit() noexcept
  -> valid_if_nonnegative<int>;

/// @brief Returns the complete version number as a tuple (major, minor, patch, commit).
/// @ingroup main_ctx
/// @see config_git_version_major
/// @see config_git_version_minor
/// @see config_git_version_patch
/// @see config_git_version_commit
export [[nodiscard]] auto config_git_version_tuple() noexcept
  -> std::tuple<int, int, int, int>;
//------------------------------------------------------------------------------
// compiler
//------------------------------------------------------------------------------
/// @brief Returns a short name of the target architecture.
/// @ingroup main_ctx
/// @see architecture_name
export [[nodiscard]] auto architecture_tag() noexcept
  -> valid_if_not_empty<string_view>;

/// @brief Returns a descriptive name of the target architecture.
/// @ingroup main_ctx
/// @see architecture_tag
export [[nodiscard]] auto architecture_name() noexcept
  -> valid_if_not_empty<string_view>;

/// @brief Returns the name of the compiler used to build the binaries.
/// @ingroup main_ctx
/// @see compiler_version_major
/// @see compiler_version_minor
/// @see compiler_version_patch
export [[nodiscard]] auto compiler_name() noexcept
  -> valid_if_not_empty<string_view>;

/// @brief Returns the major version number of the compiler.
/// @ingroup main_ctx
/// @see compiler_name
/// @see compiler_version_minor
/// @see compiler_version_patch
export [[nodiscard]] auto compiler_version_major() noexcept
  -> valid_if_nonnegative<int>;

/// @brief Returns the minor version number of the compiler.
/// @ingroup main_ctx
/// @see compiler_name
/// @see compiler_version_major
/// @see compiler_version_patch
export [[nodiscard]] auto compiler_version_minor() noexcept
  -> valid_if_nonnegative<int>;

/// @brief Returns the version patch number of the compiler.
/// @ingroup main_ctx
/// @see compiler_name
/// @see compiler_version_major
/// @see compiler_version_minor
export [[nodiscard]] auto compiler_version_patch() noexcept
  -> valid_if_nonnegative<int>;

export [[nodiscard]] auto compiler_version_tuple() noexcept
  -> std::tuple<int, int, int> {
    return {
      compiler_version_major().value_or(-1),
      compiler_version_minor().value_or(-1),
      compiler_version_patch().value_or(-1)};
}
//------------------------------------------------------------------------------
/// @brief Class providing compiler and architecture information.
/// @ingroup main_context
export class compiler_info
  : public structural_core<
      // arch. name, compiler name, compiler version numbers
      std::tuple<std::string, std::string, std::tuple<int, int, int>>> {

    using base = structural_core<
      std::tuple<std::string, std::string, std::tuple<int, int, int>>>;

public:
    /// @brief Default constructor, populates the class with detected values.
    compiler_info() noexcept
      : base{
          {to_string(eagine::architecture_name().value_anyway()),
           to_string(eagine::compiler_name().value_anyway()),
           {compiler_version_major().value_anyway(),
            compiler_version_minor().value_anyway(),
            compiler_version_patch().value_anyway()}}} {}

    compiler_info(nothing_t) noexcept {}

    /// @brief Returns the architecture name.
    /// @see name
    [[nodiscard]] auto architecture_name() const noexcept
      -> valid_if_not_empty<string_view> {
        return {std::get<0>(get_structure())};
    }

    /// @brief Returns the compiler name.
    /// @see architecture_name
    [[nodiscard]] auto name() const noexcept
      -> valid_if_not_empty<string_view> {
        return {std::get<1>(get_structure())};
    }

    /// @brief Returns the compiler version numbers in a single tuple.
    /// @see name
    /// @see version_major
    /// @see version_minor
    /// @see version_patch
    [[nodiscard]] auto version_tuple() const noexcept
      -> const std::tuple<int, int, int>& {
        return std::get<2>(get_structure());
    }

    /// @brief Returns the compiler major version number.
    /// @see name
    /// @see version_tuple
    /// @see version_minor
    /// @see version_patch
    [[nodiscard]] auto version_major() const noexcept
      -> valid_if_nonnegative<int> {
        return {std::get<0>(version_tuple())};
    }

    /// @brief Returns the compiler minor version number.
    /// @see name
    /// @see version_tuple
    /// @see version_major
    /// @see version_patch
    [[nodiscard]] auto version_minor() const noexcept
      -> valid_if_nonnegative<int> {
        return {std::get<1>(version_tuple())};
    }

    /// @brief Returns the compiler patch number.
    /// @see name
    /// @see version_tuple
    /// @see version_major
    /// @see version_minor
    [[nodiscard]] auto version_patch() const noexcept
      -> valid_if_nonnegative<int> {
        return {std::get<2>(version_tuple())};
    }
};
//------------------------------------------------------------------------------
// os
//------------------------------------------------------------------------------
/// @brief Returns the name of the OS on which the executable is built.
/// @ingroup main_ctx
/// @see config_os_code_name
export [[nodiscard]] auto config_os_name() noexcept
  -> valid_if_not_empty<string_view>;

/// @brief Returns the code-name of the OS on which the executable is built.
/// @ingroup main_ctx
/// @see config_os_name
export [[nodiscard]] auto config_os_code_name() noexcept
  -> valid_if_not_empty<string_view>;
//------------------------------------------------------------------------------
// version
//------------------------------------------------------------------------------
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
    [[nodiscard]] static auto git() noexcept -> version_info {
        return {config_git_version_tuple()};
    }

    /// @brief Creates an instance of git source version info.
    /// @see git
    [[nodiscard]] static auto compiler() noexcept -> version_info {
        return {compiler_version_tuple()};
    }

    /// @brief Returns the project version numbers in a single tuple.
    /// @see version_major
    /// @see version_minor
    /// @see version_patch
    /// @see version_commit
    [[nodiscard]] auto version_tuple() const noexcept
      -> const std::tuple<int, int, int, int>& {
        return get_structure();
    }

    /// @brief Returns the project major version number.
    /// @see version_tuple
    /// @see version_minor
    /// @see version_patch
    /// @see version_commit
    [[nodiscard]] auto version_major() const noexcept
      -> valid_if_nonnegative<int> {
        return {std::get<0>(version_tuple())};
    }

    /// @brief Returns the project minor version number.
    /// @see version_tuple
    /// @see version_major
    /// @see version_patch
    /// @see version_commit
    [[nodiscard]] auto version_minor() const noexcept
      -> valid_if_nonnegative<int> {
        return {std::get<1>(version_tuple())};
    }

    /// @brief Returns the project patch number.
    /// @see version_tuple
    /// @see version_major
    /// @see version_minor
    /// @see version_commit
    [[nodiscard]] auto version_patch() const noexcept
      -> valid_if_nonnegative<int> {
        return {std::get<2>(version_tuple())};
    }

    /// @brief Returns the project commit number.
    /// @see version_tuple
    /// @see version_major
    /// @see version_minor
    /// @see version_patch
    [[nodiscard]] auto version_commit() const noexcept
      -> valid_if_nonnegative<int> {
        return {std::get<3>(version_tuple())};
    }

    /// @brief Indicates if major and minor version numbers are known.
    /// @see version_major
    /// @see version_minor
    [[nodiscard]] auto has_value() const noexcept -> bool {
        return version_major() and version_minor();
    }

    /// @brief Checks if the project build version is at least as specified.
    [[nodiscard]] auto version_at_least(int major, int minor) const -> tribool;

    /// @brief Checks if the project build version is at least as specified.
    [[nodiscard]] auto version_at_least(int major, int minor, int patch) const
      -> tribool;

    /// @brief Checks if the project build version is at least as specified.
    [[nodiscard]] auto version_at_least(
      int major,
      int minor,
      int patch,
      int commit) const -> tribool;

    /// @brief Checks if the project build version is at least as specified.
    [[nodiscard]] auto version_at_least(const string_view) const -> tribool;
};
//------------------------------------------------------------------------------
// build
//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------
} // namespace eagine
