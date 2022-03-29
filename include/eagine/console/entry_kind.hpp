/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_CONSOLE_ENTRY_KIND_HPP
#define EAGINE_CONSOLE_ENTRY_KIND_HPP

#include "../reflect/enumerators.hpp"

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Console entry kind enumeration.
/// @ingroup console
enum class console_entry_kind {
    /// @brief Statistic entries.
    stat,
    /// @brief Informational entries.
    info,
    /// @brief Warning entries, indicating potential problems.
    warning,
    /// @brief Error entries, indicating serious problems.
    error
};

#if !EAGINE_CXX_REFLECTION
template <typename Selector>
constexpr auto enumerator_mapping(
  const type_identity<console_entry_kind>,
  const Selector) noexcept {
    return enumerator_map_type<console_entry_kind, 4>{
      {{"stat", console_entry_kind::stat},
       {"info", console_entry_kind::info},
       {"warning", console_entry_kind::warning},
       {"error", console_entry_kind::error}}};
}
#endif
//------------------------------------------------------------------------------
} // namespace eagine

#endif
