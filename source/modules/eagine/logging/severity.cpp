/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.logging:severity;

import eagine.core.reflection;
import <cstdint>;
import <type_traits>;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Log event severity enumeration.
/// @ingroup logging
export enum class log_event_severity : std::uint8_t {
    /// @brief Backtracing log entries (the lowest severity).
    backtrace,
    /// @brief Trace log entries.
    trace,
    /// @brief Debug log entries.
    debug,
    /// @brief Statistic log entries.
    stat,
    /// @brief Informational log entries.
    info,
    /// @brief Warning log entries, indicating potential problems.
    warning,
    /// @brief Error log entries, indicating serious problems.
    error,
    /// @brief Fatal error log entries, indicating problem requiring termination.
    fatal
};

export template <typename Selector>
constexpr auto enumerator_mapping(
  const std::type_identity<log_event_severity>,
  const Selector) noexcept {
    return enumerator_map_type<log_event_severity, 8>{
      {{"backtrace", log_event_severity::backtrace},
       {"trace", log_event_severity::trace},
       {"debug", log_event_severity::debug},
       {"stat", log_event_severity::stat},
       {"info", log_event_severity::info},
       {"warning", log_event_severity::warning},
       {"error", log_event_severity::error},
       {"fatal", log_event_severity::fatal}}};
}
//------------------------------------------------------------------------------
} // namespace eagine
