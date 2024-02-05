/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
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
/// @brief Returns log_event_severity increased by one level.
/// @relates log_event_severity
/// @see decreased
export auto increased(log_event_severity) noexcept -> log_event_severity;

/// @brief Returns log_event_severity increased by one level.
/// @relates log_event_severity
/// @see increased
export auto decreased(log_event_severity) noexcept -> log_event_severity;
//------------------------------------------------------------------------------
/// @brief Log back-end lockable.
/// @ingroup logging
export enum class log_backend_lock : std::uint8_t {
    /// @brief No thread lock.
    none,
    /// @brief Mutex.
    mutex,
    /// @brief Spinlock.
    spinlock
};

export template <typename Selector>
constexpr auto enumerator_mapping(
  const std::type_identity<log_backend_lock>,
  const Selector) noexcept {
    return enumerator_map_type<log_backend_lock, 3>{
      {{"none", log_backend_lock::none},
       {"mutex", log_backend_lock::mutex},
       {"spinlock", log_backend_lock::spinlock}}};
}
//------------------------------------------------------------------------------
/// @brief Log data format.
/// @ingroup logging
export enum class log_data_format : std::uint8_t {
    /// @brief XML format.
    xml,
    /// @brief JSON format.
    json
};

export template <typename Selector>
constexpr auto enumerator_mapping(
  const std::type_identity<log_data_format>,
  const Selector) noexcept {
    return enumerator_map_type<log_data_format, 2>{
      {{"xml", log_data_format::xml}, {"json", log_data_format::json}}};
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

    /// @brief Increases the verbosity of log messages.
    /// @see severity
    virtual void make_more_verbose() noexcept = 0;

    /// @brief Decreases the verbosity of log messages.
    /// @see severity
    virtual void make_less_verbose() noexcept = 0;

    /// @brief Returns a pointer to the actual backend to be used by an log_entry.
    /// @param source the identifier of the source logger object.
    /// @param severity the log level or severity of the log event.
    virtual auto entry_backend(const log_event_severity severity) noexcept
      -> logger_backend* = 0;

    /// @brief Starts the current log.
    virtual void begin_log() noexcept = 0;

    virtual auto register_time_interval(
      const identifier tag,
      const logger_instance_id) noexcept -> time_interval_id = 0;

    /// @brief Start of time interval measurement.
    /// @see time_interval_end
    virtual void time_interval_begin(const time_interval_id) noexcept = 0;

    /// @brief End of time interval measurement.
    /// @see time_interval_begin
    virtual void time_interval_end(const time_interval_id) noexcept = 0;

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
// Log output stream
//------------------------------------------------------------------------------
struct log_output_stream : interface<log_output_stream> {
    virtual void begin_stream(
      const memory::const_block&,
      const memory::const_block&) noexcept = 0;
    void begin_stream(
      const string_view stream_header,
      const string_view entry_separator) noexcept {
        begin_stream(as_bytes(stream_header), as_bytes(entry_separator));
    }

    virtual void begin_entry() noexcept = 0;

    virtual void entry_append(const memory::const_block&) noexcept = 0;
    void entry_append(const string_view entry_content) noexcept {
        entry_append(as_bytes(entry_content));
    }
    void entry_append(const memory::buffer& entry_content) noexcept {
        entry_append(view(entry_content));
    }

    virtual void finish_entry() noexcept = 0;

    virtual void finish_stream(const memory::const_block&) noexcept = 0;
    void finish_stream(const string_view stream_footer) noexcept {
        finish_stream(as_bytes(stream_footer));
    }
};
//------------------------------------------------------------------------------
auto make_std_output_stream(std::ostream&) -> unique_holder<log_output_stream>;
auto make_asio_local_output_stream(const string_view)
  -> unique_holder<log_output_stream>;
auto make_asio_tcpipv4_output_stream(const string_view)
  -> unique_holder<log_output_stream>;
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
// backend getters
//------------------------------------------------------------------------------
export auto make_null_log_backend() -> unique_holder<logger_backend>;
//------------------------------------------------------------------------------
auto make_syslog_log_backend(
  const log_stream_info&,
  const log_backend_lock lock_type) -> unique_holder<logger_backend>;
//------------------------------------------------------------------------------
auto make_json_log_backend(
  const log_stream_info&,
  shared_holder<log_output_stream>,
  const log_backend_lock lock_type) -> unique_holder<logger_backend>;

auto make_xml_log_backend(
  const log_stream_info&,
  shared_holder<log_output_stream>,
  const log_backend_lock lock_type) -> unique_holder<logger_backend>;
//------------------------------------------------------------------------------
auto make_proxy_log_backend(log_stream_info info)
  -> unique_holder<logger_backend>;
//------------------------------------------------------------------------------
auto make_asio_local_log_backend(
  string_view addr,
  const log_stream_info&,
  const log_data_format format,
  const log_backend_lock lock_type) -> unique_holder<logger_backend>;

auto make_asio_tcpipv4_log_backend(
  string_view addr,
  const log_stream_info&,
  const log_data_format format,
  const log_backend_lock lock_type) -> unique_holder<logger_backend>;
//------------------------------------------------------------------------------
auto make_ostream_log_backend(
  std::ostream&,
  const log_stream_info& info,
  const log_data_format format,
  const log_backend_lock lock_type) -> unique_holder<logger_backend>;
//------------------------------------------------------------------------------
export class log_backend_error : public std::system_error {
public:
    log_backend_error(std::system_error& orig) noexcept
      : std::system_error{std::move(orig)} {}
};
//------------------------------------------------------------------------------
/// @brief Class representing a single log time interval measurement.
/// @ingroup logging
/// @note Do not use directly, use logger instead.
class log_time_interval {
public:
    log_time_interval(
      const time_interval_id id,
      logger_backend* backend) noexcept
      : _id{id}
      , _backend{backend} {
        if(_backend) {
            _backend->time_interval_begin(_id);
        }
    }

    ~log_time_interval() noexcept {
        if(_backend) {
            _backend->time_interval_end(_id);
        }
    }

private:
    const time_interval_id _id;
    logger_backend* const _backend;
};
//------------------------------------------------------------------------------
} // namespace eagine
