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

import <string>;
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
    auto architecture_name() const noexcept -> valid_if_not_empty<string_view> {
        return {std::get<0>(get_structure())};
    }

    /// @brief Returns the compiler name.
    /// @see architecture_name
    auto name() const noexcept -> valid_if_not_empty<string_view> {
        return {std::get<1>(get_structure())};
    }

    /// @brief Returns the compiler version numbers in a single tuple.
    /// @see name
    /// @see version_major
    /// @see version_minor
    /// @see version_patch
    auto version_tuple() const noexcept -> const std::tuple<int, int, int>& {
        return std::get<2>(get_structure());
    }

    /// @brief Returns the compiler major version number.
    /// @see name
    /// @see version_tuple
    /// @see version_minor
    /// @see version_patch
    auto version_major() const noexcept -> valid_if_nonnegative<int> {
        return {std::get<0>(version_tuple())};
    }

    /// @brief Returns the compiler minor version number.
    /// @see name
    /// @see version_tuple
    /// @see version_major
    /// @see version_patch
    auto version_minor() const noexcept -> valid_if_nonnegative<int> {
        return {std::get<1>(version_tuple())};
    }

    /// @brief Returns the compiler patch number.
    /// @see name
    /// @see version_tuple
    /// @see version_major
    /// @see version_minor
    auto version_patch() const noexcept -> valid_if_nonnegative<int> {
        return {std::get<2>(version_tuple())};
    }
};

} // namespace eagine
