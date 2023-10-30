/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.logging:backend;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.runtime;
import eagine.core.identifier;
import eagine.core.reflection;
import :entry_arg;

namespace eagine {

/// @brief Logger object instance id type.
/// @ingroup logging
export using logger_instance_id = std::uintptr_t;

/// @brief Time interval measurement id type.
/// @ingroup logging
export using time_interval_id = std::uintptr_t;

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
    /// @brief Entries informing about potential process state change.
    change,
    /// @brief log entries, indicating potential problems.
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
    return enumerator_map_type<log_event_severity, 9>{
      {{"backtrace", log_event_severity::backtrace},
       {"trace", log_event_severity::trace},
       {"debug", log_event_severity::debug},
       {"stat", log_event_severity::stat},
       {"info", log_event_severity::info},
       {"change", log_event_severity::change},
       {"warning", log_event_severity::warning},
       {"error", log_event_severity::error},
       {"fatal", log_event_severity::fatal}}};
}
//------------------------------------------------------------------------------
/// @brief Structure used to supply initial log stream information to a logger.
/// @ingroup logging
export struct log_stream_info {
    /// @brief Unique session identity name.
    std::string session_identity;
    /// @brief Unique stream identity name.
    std::string log_identity;
    /// @brief The minimum severity of log messages.
    log_event_severity min_severity;
};
//------------------------------------------------------------------------------
/// @brief Interface for logging backend implementations.
/// @ingroup logging
export struct logger_backend : interface<logger_backend> {
    virtual auto configure(basic_config_intf&) -> bool {
        return false;
    }

    /// @brief The memory allocator used by the logger backend.
    virtual auto allocator() noexcept -> memory::shared_byte_allocator = 0;

    /// @brief The backend type identifier.
    virtual auto type_id() noexcept -> identifier = 0;

    /// @brief Returns a pointer to the actual backend to be used by an log_entry.
    /// @param source the identifier of the source logger object.
    /// @param severity the log level or severity of the log event.
    virtual auto entry_backend(const log_event_severity severity) noexcept
      -> logger_backend* = 0;

    /// @brief Starts the current log.
    virtual void begin_log() noexcept = 0;

    /// @brief Start of time interval measurement.
    /// @see time_interval_end
    virtual void time_interval_begin(
      const identifier,
      const logger_instance_id,
      const time_interval_id) noexcept = 0;

    /// @brief End of time interval measurement.
    /// @see time_interval_begin
    virtual void time_interval_end(
      const identifier,
      const logger_instance_id,
      const time_interval_id) noexcept = 0;

    /// @brief Sets the user-readable description for the logger object.
    virtual void set_description(
      const identifier source,
      const logger_instance_id instance,
      const string_view display_name,
      const string_view description) noexcept = 0;

    /// @brief Declares a state delimited by a pair of messages with specified tags.
    /// @param source the identifier of the source logger object.
    /// @param state_tag the identifier of the declared state.
    /// @param begin_tag the tag of the message that begins the declared state.
    /// @param end_tag the tag of the message that ends the declared state.
    /// @see active_state
    virtual void declare_state(
      const identifier source,
      const identifier state_tag,
      const identifier begin_tag,
      const identifier end_tag) noexcept = 0;

    /// @brief Indicates that the specified state is considered an active state.
    /// @param source the identifier of the source logger object.
    /// @param state_tag the identifier of the declared state.
    /// @see declare_state
    /// An active state is indicating that the application is working on it's
    /// main task and is initialized and prepared to handle input.
    virtual void active_state(
      const identifier source,
      const identifier state_tag) noexcept = 0;

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

    template <typename X>
    constexpr auto add_identifier(
      const identifier arg,
      const identifier tag,
      placeholder_expression<X> e) noexcept {
        return placeholder_expression{[=, this](auto&&... args) {
            this->add_identifier(arg, tag, e(decltype(args)(args)...));
        }};
    }

    virtual void add_message_id(
      const identifier arg,
      const identifier tag,
      const message_id) noexcept = 0;

    template <typename X>
    constexpr auto add_message_id(
      const identifier arg,
      const identifier tag,
      placeholder_expression<X> e) noexcept {
        return placeholder_expression{[=, this](auto&&... args) {
            this->add_message_id(arg, tag, e(decltype(args)(args)...));
        }};
    }

    /// @brief Add argument with boolean value.
    /// @param arg the argument name identifier.
    /// @param tag the argument type identifier.
    /// @param value the value of the argument.
    virtual void add_bool(
      const identifier arg,
      const identifier tag,
      const bool value) noexcept = 0;

    template <typename X>
    constexpr auto add_bool(
      const identifier arg,
      const identifier tag,
      placeholder_expression<X> e) noexcept {
        return placeholder_expression{[=, this](auto&&... args) {
            this->add_bool(arg, tag, e(decltype(args)(args)...));
        }};
    }

    /// @brief Add argument with signed integer value.
    /// @param arg the argument name identifier.
    /// @param tag the argument type identifier.
    /// @param value the value of the argument.
    virtual void add_integer(
      const identifier arg,
      const identifier tag,
      const std::intmax_t value) noexcept = 0;

    template <typename X>
    constexpr auto add_integer(
      const identifier arg,
      const identifier tag,
      placeholder_expression<X> e) noexcept {
        return placeholder_expression{[=, this](auto&&... args) {
            this->add_integer(arg, tag, e(decltype(args)(args)...));
        }};
    }

    /// @brief Add argument with unsigned integer value.
    /// @param arg the argument name identifier.
    /// @param tag the argument type identifier.
    /// @param value the value of the argument.
    virtual void add_unsigned(
      const identifier arg,
      const identifier tag,
      const std::uintmax_t value) noexcept = 0;

    template <typename X>
    constexpr auto add_unsigned(
      const identifier arg,
      const identifier tag,
      placeholder_expression<X> e) noexcept {
        return placeholder_expression{[=, this](auto&&... args) {
            this->add_unsigned(arg, tag, e(decltype(args)(args)...));
        }};
    }

    /// @brief Add argument with floating-point value.
    /// @param arg the argument name identifier.
    /// @param tag the argument type identifier.
    /// @param value the value of the argument.
    virtual void add_float(
      const identifier arg,
      const identifier tag,
      const float value) noexcept = 0;

    template <typename X>
    constexpr auto add_float(
      const identifier arg,
      const identifier tag,
      placeholder_expression<X> e) noexcept {
        return placeholder_expression{[=, this](auto&&... args) {
            this->add_float(arg, tag, e(decltype(args)(args)...));
        }};
    }

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

    template <typename X>
    constexpr auto add_duration(
      const identifier arg,
      const identifier tag,
      placeholder_expression<X> e) noexcept {
        return placeholder_expression{[=, this](auto&&... args) {
            this->add_duration(arg, tag, e(decltype(args)(args)...));
        }};
    }

    /// @brief Add argument with string value.
    /// @param arg the argument name identifier.
    /// @param tag the argument type identifier.
    /// @param value the value of the argument.
    virtual void add_string(
      const identifier arg,
      const identifier tag,
      const string_view value) noexcept = 0;

    template <typename X>
    constexpr auto add_string(
      const identifier arg,
      const identifier tag,
      placeholder_expression<X> e) noexcept {
        return placeholder_expression{[=, this](auto&&... args) {
            this->add_string(arg, tag, e(decltype(args)(args)...));
        }};
    }

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
    /// @see has_entry_adapter_v
    template <adapted_for_log_entry T>
    void add_adapted(const identifier arg, const T& value) {
        adapt_entry_arg(arg, value)(*this);
    }

    template <typename X>
    constexpr auto add_adapted(
      const identifier arg,
      placeholder_expression<X> e) noexcept {
        return placeholder_expression{[=, this](auto&&... args) {
            this->add_adapted(arg, e(decltype(args)(args)...));
        }};
    }

    /// @brief Finishes the current logging message.
    virtual void finish_message() noexcept = 0;

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

    /// @brief Indicates that the running process is alive.
    virtual void heartbeat() noexcept = 0;

    /// @brief Finishes the current log.
    virtual void finish_log() noexcept = 0;
};
//------------------------------------------------------------------------------
// backend getters
//------------------------------------------------------------------------------
export auto make_null_log_backend() -> unique_holder<logger_backend>;
//------------------------------------------------------------------------------
export auto make_asio_local_ostream_log_backend_mutex(const log_stream_info&)
  -> unique_holder<logger_backend>;
export auto make_asio_local_ostream_log_backend_spinlock(const log_stream_info&)
  -> unique_holder<logger_backend>;

export auto make_asio_local_ostream_log_backend_mutex(
  string_view addr,
  const log_stream_info&) -> unique_holder<logger_backend>;
export auto make_asio_local_ostream_log_backend_spinlock(
  string_view addr,
  const log_stream_info&) -> unique_holder<logger_backend>;

export auto make_asio_tcpipv4_ostream_log_backend_mutex(
  string_view addr,
  const log_stream_info&) -> unique_holder<logger_backend>;
export auto make_asio_tcpipv4_ostream_log_backend_spinlock(
  string_view addr,
  const log_stream_info&) -> unique_holder<logger_backend>;
//------------------------------------------------------------------------------
export class log_backend_error : public std::system_error {
public:
    log_backend_error(std::system_error& orig) noexcept
      : std::system_error{std::move(orig)} {}
};
//------------------------------------------------------------------------------
export auto make_proxy_log_backend(log_stream_info info)
  -> unique_holder<logger_backend>;
//------------------------------------------------------------------------------
export auto make_syslog_log_backend_mutex(const log_stream_info&)
  -> unique_holder<logger_backend>;
export auto make_syslog_log_backend_spinlock(const log_stream_info&)
  -> unique_holder<logger_backend>;
//------------------------------------------------------------------------------
/// @brief Class representing a single log time interval measurement.
/// @ingroup logging
/// @note Do not use directly, use logger instead.
class log_time_interval {

public:
    auto interval_id() const noexcept {
        return reinterpret_cast<time_interval_id>(this);
    }

    log_time_interval(
      const identifier label,
      const logger_instance_id inst,
      logger_backend* backend) noexcept
      : _label{label}
      , _instance_id{inst}
      , _backend{backend} {
        if(_backend) {
            _backend->time_interval_begin(_label, _instance_id, interval_id());
        }
    }

    ~log_time_interval() noexcept {
        if(_backend) {
            _backend->time_interval_end(_label, _instance_id, interval_id());
        }
    }

private:
    const identifier _label;
    const logger_instance_id _instance_id;
    logger_backend* const _backend;
};
//------------------------------------------------------------------------------
} // namespace eagine
