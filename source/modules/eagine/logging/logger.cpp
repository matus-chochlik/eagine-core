/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.logging:logger;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.identifier;
import eagine.core.valid_if;
import eagine.core.utility;
import eagine.core.runtime;
import eagine.core.units;
import :config;
import :backend;
import :entry;

namespace eagine {
//------------------------------------------------------------------------------
export struct time_interval_handle {
    const time_interval_id _id;
};
//------------------------------------------------------------------------------
/// @brief Basic template for logger objects.
/// @ingroup logging
export class basic_logger {
public:
    /// @brief The entry type for the specified log_event_severity.
    /// @see log_entry
    /// @see no_log_entry
    template <log_event_severity severity>
    using entry_type = std::
      conditional_t<is_log_level_enabled_v<severity>, log_entry, no_log_entry>;

    auto share_backend() const noexcept {
        return _backend;
    }

    /// @brief Returns a pointer to the back-end of this logger object.
    auto backend() const noexcept -> optional_reference<logger_backend> {
        return _backend;
    }

    /// @brief Indicates that this process is alive.
    void heartbeat() const noexcept;

    /// @brief Returns the unique id of this logger instance.
    auto instance_id() const noexcept -> logger_instance_id {
        return reinterpret_cast<logger_instance_id>(this);
    }

    /// @brief Returns an identifier for an interval time measurement.
    auto register_time_interval(const identifier label) const noexcept
      -> time_interval_handle;

    /// @brief Creates object the lifetime of this is measured and logged.
    auto measure_time_interval(
      const time_interval_handle handle,
      log_event_severity severity) const noexcept -> log_time_interval {
        return {handle._id, _entry_backend(severity)};
    }

    /// @brief Creates object the lifetime of this is measured and logged.
    auto measure_time_interval(const time_interval_handle handle) const noexcept
      -> log_time_interval {
        return measure_time_interval(handle, log_event_severity::stat);
    }

    auto configure(basic_config_intf& config) const -> bool;

protected:
    constexpr basic_logger() noexcept = default;

    basic_logger(shared_holder<logger_backend> backend) noexcept
      : _backend{std::move(backend)} {}

    void begin_log() noexcept;
    void finish_log() noexcept;

    void set_description(
      const identifier source,
      const string_view display_name,
      const string_view description) const noexcept;

    void declare_state(
      const identifier source,
      const identifier state_tag,
      const identifier begin_tag,
      const identifier end_tag) const noexcept;

    void active_state(const identifier source, const identifier state_tag)
      const noexcept;

    auto make_log_entry(
      const identifier source,
      const log_event_severity severity,
      const std::true_type,
      const string_view format) const noexcept -> log_entry {
        return {
          source, instance_id(), severity, format, _entry_backend(severity)};
    }

    constexpr auto make_log_entry(
      const identifier,
      const log_event_severity,
      const std::false_type,
      const string_view) const noexcept -> no_log_entry {
        return {};
    }

    template <log_event_severity severity>
    auto make_log_entry(
      const identifier source,
      const log_event_severity_constant<severity>,
      const string_view format) const noexcept -> entry_type<severity> {
        return make_log_entry(
          source, severity, is_log_level_enabled_t<severity>{}, format);
    }

    auto make_log_entry(
      const identifier source,
      const log_event_severity severity,
      const string_view format) const noexcept -> log_entry {
        return {
          source, instance_id(), severity, format, _entry_backend(severity)};
    }

    auto make_log_entry(
      repeating_log_entry_control& control,
      const identifier source,
      const log_event_severity severity,
      const string_view format) const noexcept -> log_entry {
        logger_backend* entry_backend{nullptr};
        if(control.should_be_logged()) [[unlikely]] {
            entry_backend = _entry_backend(severity);
            control.was_logged();
        }
        return {source, instance_id(), severity, format, entry_backend};
    }

    auto log_fatal(const identifier source, const string_view format)
      const noexcept {
        return make_log_entry(
          source,
          log_event_severity_constant<log_event_severity::fatal>{},
          format);
    }

    auto log_fatal(
      repeating_log_entry_control& control,
      const identifier source,
      const string_view format) const noexcept {
        return make_log_entry(
          control, source, log_event_severity::fatal, format);
    }

    auto log_error(const identifier source, const string_view format)
      const noexcept {
        return make_log_entry(
          source,
          log_event_severity_constant<log_event_severity::error>{},
          format);
    }

    auto log_error(
      repeating_log_entry_control& control,
      const identifier source,
      const string_view format) const noexcept {
        return make_log_entry(
          control, source, log_event_severity::error, format);
    }

    auto log_warning(const identifier source, const string_view format)
      const noexcept {
        return make_log_entry(
          source,
          log_event_severity_constant<log_event_severity::warning>{},
          format);
    }

    auto log_warning(
      repeating_log_entry_control& control,
      const identifier source,
      const string_view format) const noexcept {
        return make_log_entry(
          control, source, log_event_severity::warning, format);
    }

    auto log_change(const identifier source, const string_view format)
      const noexcept {
        return make_log_entry(
          source,
          log_event_severity_constant<log_event_severity::change>{},
          format);
    }

    auto log_info(const identifier source, const string_view format)
      const noexcept {
        return make_log_entry(
          source,
          log_event_severity_constant<log_event_severity::info>{},
          format);
    }

    auto log_stat(const identifier source, const string_view format)
      const noexcept {
        return make_log_entry(
          source,
          log_event_severity_constant<log_event_severity::stat>{},
          format);
    }

    auto log_debug(const identifier source, const string_view format)
      const noexcept {
        return make_log_entry(
          source,
          log_event_severity_constant<log_event_severity::debug>{},
          format);
    }

    auto log_trace(const identifier source, const string_view format)
      const noexcept {
        return make_log_entry(
          source,
          log_event_severity_constant<log_event_severity::trace>{},
          format);
    }

    auto log_backtrace(const identifier source, const string_view format)
      const noexcept {
        return make_log_entry(
          source,
          log_event_severity_constant<log_event_severity::backtrace>{},
          format);
    }

    auto log_lifetime(const identifier source, const string_view format)
      const noexcept {
        // TODO: switch depending on Debug/Release ?
        return log_info(source, format);
    }

    void log_chart_sample(
      const identifier source,
      const identifier series,
      const float value) const noexcept;

    auto _entry_backend(const log_event_severity severity) const noexcept
      -> logger_backend* {
        if(is_log_level_enabled(severity)) {
            if(auto lbe{backend()}) {
                return lbe->entry_backend(severity);
            }
        }
        return nullptr;
    }

private:
    shared_holder<logger_backend> _backend;
};
//------------------------------------------------------------------------------
/// @brief Basic template for logging objects.
/// @ingroup logging
/// @see logger
///
/// This class is used as the base class for other classes that need to do
/// logging. Prefer using this class instead of logger in such cases.
export class named_logging_object : public basic_logger {
    using base = basic_logger;

public:
    /// @brief The entry type for the specified log_event_severity.
    /// @see log_entry
    /// @see no_log_entry
    template <log_event_severity severity>
    using entry_type = std::
      conditional_t<is_log_level_enabled_v<severity>, log_entry, no_log_entry>;

    using base::log_lifetime;

    named_logging_object(const identifier id) noexcept
      : _object_id{id} {}

    /// @brief Constructor from identifier and back-end object.
    /// @note For internal use-only.
    named_logging_object(
      const identifier id,
      shared_holder<logger_backend> backend) noexcept
      : base{std::move(backend)}
      , _object_id{id} {}

    /// @brief Constructor from logger id and parent logging object.
    named_logging_object(
      const identifier id,
      const named_logging_object& parent) noexcept;

    /// @brief Construct logging object without back-end.
    constexpr named_logging_object() noexcept = default;

    /// @brief Move constructor.
    named_logging_object(named_logging_object&& temp) noexcept;

    /// @brief Copy constructor.
    named_logging_object(const named_logging_object& that) noexcept;

    /// @brief Move assignment operator.
    auto operator=(named_logging_object&&) noexcept -> named_logging_object&;

    /// @brief Copy assignment operator.
    auto operator=(const named_logging_object&) -> named_logging_object&;

    ~named_logging_object() noexcept;

    /// @brief Returns the identifier of this logging object.
    constexpr auto object_id() const noexcept {
        return _object_id;
    }

    ///@brief Sets the human-readable name and description of this object.
    void object_description(
      const string_view display_name,
      const string_view description) const noexcept {
        base::set_description(_object_id, display_name, description);
    }

    /// @brief Declares a possible special state delimited by pair of messages.
    void declare_state(
      const identifier state_tag,
      const identifier begin_tag,
      const identifier end_tag) const noexcept {
        base::declare_state(_object_id, state_tag, begin_tag, end_tag);
    }

    using base::active_state;

    /// @brief Declares a possible active state.
    void active_state(const identifier state_tag) const noexcept {
        base::active_state(_object_id, state_tag);
    }

    /// @brief Create a log message entry with specified severity and format.
    auto log(const log_event_severity severity, const string_view format)
      const noexcept {
        return base::make_log_entry(_object_id, severity, format);
    }

    /// @brief Create a log message entry for fatal error, with specified format.
    /// @see log_event_severity
    auto log_fatal(const string_view format) const noexcept {
        return base::log_fatal(_object_id, format);
    }

    /// @brief Create a log message entry for fatal error, with specified format.
    /// @see log_event_severity
    auto log_fatal(
      repeating_log_entry_control& control,
      const string_view format) const noexcept {
        return base::log_fatal(control, _object_id, format);
    }

    /// @brief Create a log message entry for error, with specified format.
    /// @see log_event_severity
    auto log_error(const string_view format) const noexcept {
        return base::log_error(_object_id, format);
    }

    /// @brief Create a log message entry for error, with specified format.
    /// @see log_event_severity
    auto log_error(
      repeating_log_entry_control& control,
      const string_view format) const noexcept {
        return base::log_error(control, _object_id, format);
    }

    /// @brief Create a log message entry for warning, with specified format.
    /// @see log_event_severity
    auto log_warning(const string_view format) const noexcept {
        return base::log_warning(_object_id, format);
    }

    /// @brief Create a log message entry for warning, with specified format.
    /// @see log_event_severity
    auto log_warning(
      repeating_log_entry_control& control,
      const string_view format) const noexcept {
        return base::log_warning(control, _object_id, format);
    }

    /// @brief Create a log message entry about state change, with specified format.
    /// @see log_event_severity
    auto log_change(const string_view format) const noexcept {
        return base::log_change(_object_id, format);
    }

    /// @brief Create a log message entry for information, with specified format.
    /// @see log_event_severity
    auto log_info(const string_view format) const noexcept {
        return base::log_info(_object_id, format);
    }

    /// @brief Create a log message entry for statistic, with specified format.
    /// @see log_event_severity
    auto log_stat(const string_view format) const noexcept {
        return base::log_stat(_object_id, format);
    }

    /// @brief Create a log message entry for debugging, with specified format.
    /// @see log_event_severity
    auto log_debug(const string_view format) const noexcept {
        return base::log_debug(_object_id, format);
    }

    /// @brief Create a log message entry for tracing, with specified format.
    /// @see log_event_severity
    auto log_trace(const string_view format) const noexcept {
        return base::log_trace(_object_id, format);
    }

    /// @brief Create a log message entry for backtracing, with specified format.
    /// @see log_event_severity
    auto log_backtrace(const string_view format) const noexcept {
        return base::log_backtrace(_object_id, format);
    }

    /// @brief Stores a new @p value in the specified chart data @p series.
    auto log_chart_sample(const identifier series, const float value)
      const noexcept -> const named_logging_object& {
        base::log_chart_sample(_object_id, series, value);
        return *this;
    }

    /// @brief Stores a new @p value in the specified chart data @p series.
    template <typename T, typename U>
    auto log_chart_sample(
      const identifier series,
      const tagged_quantity<T, U>& qty) const noexcept
      -> const named_logging_object& requires(std::is_convertible_v<T, float>) {
          log_chart_sample(series, qty.value());
          return *this;
      }

    /// @brief Stores a new @p value in the specified chart data @p series.
    template <typename T, typename P>
    auto log_chart_sample(
      const identifier series,
      const valid_if<T, P>& opt_value) const noexcept
      -> const named_logging_object& {
        if(opt_value) {
            base::log_chart_sample(_object_id, series, float(*opt_value));
        }
        return *this;
    }

protected:
    template <log_event_severity severity>
    auto make_log_entry(
      const identifier tag,
      log_event_severity_constant<severity> level,
      const string_view format) noexcept -> entry_type<severity> {
        return base::make_log_entry(_object_id, tag, level, format);
    }

    auto make_log_entry(
      const log_event_severity severity,
      const string_view format) const noexcept -> log_entry {
        return base::make_log_entry(_object_id, severity, format);
    }

private:
    identifier _object_id{};
};
//------------------------------------------------------------------------------
/// @brief Standalone logger object type.
/// @ingroup logging
/// @see named_logging_object
///
/// Consider using named_logging_object instead of logger where appropriate.
export class logger : public named_logging_object {
    using base = named_logging_object;

public:
    /// @brief Default constructor.
    constexpr logger() noexcept = default;

    /// @brief Construction from identifier and backed getter
    logger(const identifier id, shared_holder<logger_backend> backend) noexcept
      : base{id, std::move(backend)} {}

    /// @brief Construction from identifier and reference to parent object.
    logger(const identifier id, const base& parent) noexcept
      : base{id, parent} {}

    /// @brief Construction from identifier and reference to parent logger.
    logger(const identifier id, const logger& parent)
      : base{id, static_cast<const base&>(parent)} {}

    /// @brief Returns a log entry with the specified log_event_severity level.
    /// @param format the log message format string.
    /// @see fatal
    /// @see error
    /// @see warning
    /// @see info
    /// @see stat
    /// @see debug
    /// @see trace
    /// @see backtrace
    /// @see log_tagged
    template <log_event_severity severity>
    auto log(
      const log_event_severity_constant<severity> level,
      const string_view format) const noexcept -> entry_type<severity> {
        return make_log_entry(level, format);
    }

    /// @brief Returns a log entry with the specified log_event_severity.
    /// @param format the log message format string.
    /// @see fatal
    /// @see error
    /// @see warning
    /// @see info
    /// @see stat
    /// @see debug
    /// @see trace
    /// @see backtrace
    auto log(const log_event_severity severity, const string_view format)
      const noexcept {
        return make_log_entry(severity, format);
    }

    /// @brief Stores a new @p value in the specified chart data @p series.
    auto chart_sample(const identifier series, const float value) const noexcept
      -> const logger& {
        base::log_chart_sample(series, value);
        return *this;
    }

    /// @brief Stores a new @p value in the specified chart data @p series.
    template <typename T, typename U>
    auto chart_sample(
      const identifier series,
      const tagged_quantity<T, U>& qty) noexcept
      -> const logger& requires(std::is_convertible_v<T, float>) {
          log_chart_sample(series, qty.value());
          return *this;
      }

    /// @brief Stores a new @p value in the specified chart data @p series.
    template <typename T, typename P>
    auto chart_sample(const identifier series, const valid_if<T, P>& opt_value)
      const noexcept -> const logger& {
        if(opt_value) {
            base::log_chart_sample(series, float(*opt_value));
        }
        return *this;
    }

    /// @brief Returns a fatal error log entry.
    /// @param format the log message format string.
    /// @see log
    auto fatal(const string_view format) const noexcept {
        return log_fatal(format);
    }

    /// @brief Returns an error log entry.
    /// @param format the log message format string.
    /// @see log
    auto error(const string_view format) const noexcept {
        return log_error(format);
    }

    /// @brief Returns a warning log entry.
    /// @param format the log message format string.
    /// @see log
    auto warning(const string_view format) const noexcept {
        return log_warning(format);
    }

    /// @brief Returns a statu-change log entry.
    /// @param format the log message format string.
    /// @see log
    auto change(const string_view format) const noexcept {
        return log_change(format);
    }

    /// @brief Returns an informational log entry.
    /// @param format the log message format string.
    /// @see log
    auto info(const string_view format) const noexcept {
        return log_info(format);
    }

    /// @brief Returns a statistic log entry.
    /// @param format the log message format string.
    /// @see log
    auto stat(const string_view format) const noexcept {
        return log_stat(format);
    }

    /// @brief Returns a debugging log entry.
    /// @param format the log message format string.
    /// @see log
    auto debug(const string_view format) const noexcept {
        return log_debug(format);
    }

    /// @brief Returns a tracing log entry.
    /// @param format the log message format string.
    /// @see log
    auto trace(const string_view format) const noexcept {
        return log_trace(format);
    }

    /// @brief Returns a backtracing log entry.
    /// @param format the log message format string.
    /// @see log
    auto backtrace(const string_view format) const noexcept {
        return log_backtrace(format);
    }

    /// @brief Connects a logging handler when signal_switch is flipped.
    [[nodiscard]] auto log_when_switched(signal_switch& s) const noexcept
      -> signal_binding;

private:
    void _handle_interrupt_signal(bool is_signaled) const noexcept;
};
//------------------------------------------------------------------------------
} // namespace eagine
