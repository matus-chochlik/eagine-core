/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_LOGGING_BACKEND_HPP
#define EAGINE_LOGGING_BACKEND_HPP

#include "../interface.hpp"
#include "../memory/block.hpp"
#include "../memory/shared_alloc.hpp"
#include "../message_id.hpp"
#include "../string_span.hpp"
#include "fwd.hpp"
#include "severity.hpp"
#include <chrono>
#include <cstdint>

namespace eagine {
class application_config;
//------------------------------------------------------------------------------
/// @brief Helper class used in implementation of has_log_entry_adapter_t.
/// @ingroup logging
template <typename T>
struct does_have_log_entry_adapter {
private:
    template <
      typename X,
      typename = decltype(
        adapt_log_entry_arg(std::declval<identifier>(), std::declval<X>()))>
    static auto _test(X*) -> std::true_type;
    static auto _test(...) -> std::false_type;

public:
    // NOLINTNEXTLINE(hicpp-vararg)
    using type = decltype(_test(static_cast<T*>(nullptr)));
};

/// @brief Trait indicating if there is a log entry adapter for type T.
/// @ingroup logging
/// @see has_log_entry_adapter_v
template <typename T>
using has_log_entry_adapter_t = typename does_have_log_entry_adapter<T>::type;

/// @brief Trait indicating if there is a log entry adapter for type T.
/// @ingroup logging
/// @see has_log_entry_adapter_t
template <typename T>
constexpr const bool has_log_entry_adapter_v =
  has_log_entry_adapter_t<T>::value;
//------------------------------------------------------------------------------
/// @brief Structure used to supply initial log stream information to a logger.
/// @ingroup logging
struct log_stream_info {
    /// @brief Unique stream identity name.
    std::string log_identity;
    /// @brief The minimum severity of log messages.
    log_event_severity min_severity;
};
//------------------------------------------------------------------------------
/// @brief Interface for logging backend implementations.
/// @ingroup logging
struct logger_backend : interface<logger_backend> {
    virtual auto configure(application_config&) -> bool {
        return false;
    }

    /// @brief The memory allocator used by the logger backend.
    virtual auto allocator() noexcept -> memory::shared_byte_allocator = 0;

    /// @brief The backend type identifier.
    virtual auto type_id() noexcept -> identifier = 0;

    /// @brief Returns a pointer to the actual backend to be used by an log_entry.
    /// @param source the identifier of the source logger object.
    /// @param severity the log level or severity of the log event.
    virtual auto entry_backend(
      const identifier source,
      const log_event_severity severity) noexcept -> logger_backend* = 0;

    /// @brief Enters logging scope.
    virtual void enter_scope(const identifier scope) noexcept = 0;

    /// @brief Leaves logging scope.
    virtual void leave_scope(const identifier scope) noexcept = 0;

    /// @brief Sets the user-readable description for the logger object.
    virtual void set_description(
      const identifier source,
      const logger_instance_id instance,
      const string_view display_name,
      const string_view description) noexcept = 0;

    /// @brief Begins a new logging message.
    /// @param source the identifier of the source logger object.
    /// @param tag the identifier of this message type or instance.
    /// @param instance unique instance id of the source logger object.
    /// @param severity the log level or severity of the log event.
    /// @param format the format string of the message. May contain argument placeholders.
    virtual auto begin_message(
      const identifier source,
      const identifier tag,
      const logger_instance_id instance,
      const log_event_severity severity,
      const string_view format) noexcept -> bool = 0;

    /// @brief Add valueless (name-only) argument.
    /// @param arg the argument name identifier.
    /// @param tag the argument type identifier.
    virtual void add_nothing(
      const identifier arg,
      const identifier tag) noexcept = 0;

    /// @brief Add argument with identifier value.
    /// @param arg the argument name identifier.
    /// @param tag the argument type identifier.
    /// @param value the value of the argument.
    virtual void add_identifier(
      const identifier arg,
      const identifier tag,
      const identifier value) noexcept = 0;

    virtual void add_message_id(
      const identifier arg,
      const identifier tag,
      const message_id) noexcept = 0;

    /// @brief Add argument with boolean value.
    /// @param arg the argument name identifier.
    /// @param tag the argument type identifier.
    /// @param value the value of the argument.
    virtual void add_bool(
      const identifier arg,
      const identifier tag,
      const bool value) noexcept = 0;

    /// @brief Add argument with signed integer value.
    /// @param arg the argument name identifier.
    /// @param tag the argument type identifier.
    /// @param value the value of the argument.
    virtual void add_integer(
      const identifier arg,
      const identifier tag,
      const std::intmax_t value) noexcept = 0;

    /// @brief Add argument with unsigned integer value.
    /// @param arg the argument name identifier.
    /// @param tag the argument type identifier.
    /// @param value the value of the argument.
    virtual void add_unsigned(
      const identifier arg,
      const identifier tag,
      const std::uintmax_t value) noexcept = 0;

    /// @brief Add argument with floating-point value.
    /// @param arg the argument name identifier.
    /// @param tag the argument type identifier.
    /// @param value the value of the argument.
    virtual void add_float(
      const identifier arg,
      const identifier tag,
      const float value) noexcept = 0;

    /// @brief Add argument with floating-point value with minimum and maximum.
    /// @param arg the argument name identifier.
    /// @param tag the argument type identifier.
    /// @param value the value of the argument.
    /// @param min the minimum limit for the value.
    /// @param max the maximum limit for the value.
    virtual void add_float(
      const identifier arg,
      const identifier tag,
      const float min,
      const float value,
      const float max) noexcept = 0;

    /// @brief Add argument with time duration value.
    /// @param arg the argument name identifier.
    /// @param tag the argument type identifier.
    /// @param value the value of the argument.
    virtual void add_duration(
      const identifier arg,
      const identifier tag,
      const std::chrono::duration<float> value) noexcept = 0;

    /// @brief Add argument with string value.
    /// @param arg the argument name identifier.
    /// @param tag the argument type identifier.
    /// @param value the value of the argument.
    virtual void add_string(
      const identifier arg,
      const identifier tag,
      const string_view value) noexcept = 0;

    /// @brief Add argument with BLOB value.
    /// @param arg the argument name identifier.
    /// @param tag the argument type identifier.
    /// @param value the value of the argument.
    virtual void add_blob(
      const identifier arg,
      const identifier tag,
      const memory::const_block value) noexcept = 0;

    /// @brief Add argument with value having type adaptable to log entry.
    /// @param arg the argument name identifier.
    /// @param value the value of the argument.
    /// @see has_log_entry_adapter_v
    template <typename T>
    void add_adapted(const identifier arg, const T& value) requires(
      has_log_entry_adapter_v<T>) {
        adapt_log_entry_arg(arg, value)(*this);
    }

    /// @brief Finishes the current logging message.
    virtual void finish_message() noexcept = 0;

    /// @brief Finishes the current log.
    virtual void finish_log() noexcept = 0;

    /// @brief Adds a chart/graph sample to the log.
    /// @param source the identifier of the source logger object.
    /// @param instance unique instance id of the source logger object.
    /// @param series the identifier of the chart data series.
    /// @param value the sample value.
    virtual void log_chart_sample(
      const identifier source,
      const logger_instance_id instance,
      const identifier series,
      const float value) noexcept = 0;
};
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_LOGGING_BACKEND_HPP
