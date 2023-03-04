/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#ifndef EAGINE_MIN_LOG_SEVERITY
#if EAGINE_LOW_PROFILE
#define EAGINE_MIN_LOG_SEVERITY warning
#else
#if EAGINE_DEBUG
#define EAGINE_MIN_LOG_SEVERITY trace
#else
#define EAGINE_MIN_LOG_SEVERITY stat
#endif
#endif
#endif

export module eagine.core.logging:config;

import eagine.core.build_config;
import eagine.core.types;
import :severity;
import std;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Type alias for log event severity constants.
/// @ingroup logging
/// @see min_log_severity
export template <log_event_severity severity>
using log_event_severity_constant =
  std::integral_constant<log_event_severity, severity>;
//------------------------------------------------------------------------------
/// @brief The minimal log severity constant.
/// @ingroup logging
/// @see min_log_severity_t
constexpr const log_event_severity min_log_severity =
  low_profile_build ? log_event_severity::warning
  : debug_build     ? log_event_severity::trace
                    : log_event_severity::debug;

/// @brief The minimal log severity constant.
/// @ingroup logging
/// @see min_log_severity
export using min_log_severity_t = log_event_severity_constant<min_log_severity>;
//------------------------------------------------------------------------------
/// @brief Compile-time trait indicating if the specified log severity is enabled.
/// @ingroup logging
/// @see is_log_level_enabled_v
/// @see min_log_severity_t
export template <log_event_severity severity>
using is_log_level_enabled_t =
  std::bool_constant<(severity >= min_log_severity_t::value)>;

/// @brief Compile-time trait indicating if the specified log severity is enabled.
/// @ingroup logging
/// @see is_log_level_enabled_t
export template <log_event_severity severity>
constexpr const bool is_log_level_enabled_v =
  is_log_level_enabled_t<severity>::value;
//------------------------------------------------------------------------------
/// @brief Function indicating if the specified log severity is enabled.
/// @ingroup logging
/// @see is_log_level_enabled_v
export constexpr auto is_log_level_enabled(
  const log_event_severity severity) noexcept -> bool {
    return (severity >= min_log_severity_t::value);
}
//------------------------------------------------------------------------------
} // namespace eagine
