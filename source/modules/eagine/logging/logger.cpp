/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.logging:logger;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.identifier;
import eagine.core.valid_if;
import eagine.core.runtime;
import eagine.core.units;
import :config;
import :backend;
import :entry;
import :time_interval;

namespace eagine {
//------------------------------------------------------------------------------
export class logger_shared_backend_getter {
    using This = logger_shared_backend_getter;

public:
    logger_shared_backend_getter() noexcept = default;
    logger_shared_backend_getter(This&&) noexcept = default;
    logger_shared_backend_getter(const This&) noexcept = default;
    auto operator=(This&&) = delete;
    auto operator=(const This&) = delete;

    ~logger_shared_backend_getter() noexcept = default;

    logger_shared_backend_getter(
      std::shared_ptr<logger_backend> backend) noexcept
      : _backend{std::move(backend)} {}

    void begin_log() noexcept {
        if(_backend) {
            _backend->begin_log();
        }
    }

    void finish_log() noexcept {
        if(_backend) {
            _backend->finish_log();
            _backend.reset();
        }
    }

    auto get() const noexcept -> logger_backend* {
        return _backend.get();
    }

private:
    std::shared_ptr<logger_backend> _backend{};
};
//------------------------------------------------------------------------------
/// @brief Basic template for logger objects.
/// @ingroup logging
export template <typename BackendGetter>
class basic_logger : protected BackendGetter {
public:
    /// @brief The entry type for the specified log_event_severity.
    /// @see log_entry
    /// @see no_log_entry
    template <log_event_severity severity>
    using entry_type = std::
      conditional_t<is_log_level_enabled_v<severity>, log_entry, no_log_entry>;

    /// @brief Returns a pointer to the backend of this logger object.
    auto backend() const noexcept {
        return _backend_getter().get();
    }

    /// @brief Indicates that this process is alive.
    void heartbeat() const noexcept {
        if(auto lbe{backend()}) {
            extract(lbe).heartbeat();
        }
    }

    /// @brief Returns the unique id of this logger instance.
    auto instance_id() const noexcept -> logger_instance_id {
        return reinterpret_cast<logger_instance_id>(this);
    }

    /// @brief Creates object the lifetime of this is measured and logged.
    auto measure_time_interval(
      const identifier label,
      log_event_severity severity) const noexcept -> log_time_interval {
        return {label, instance_id(), _entry_backend(severity)};
    }

    /// @brief Creates object the lifetime of this is measured and logged.
    auto measure_time_interval(const identifier label) const noexcept
      -> log_time_interval {
        return measure_time_interval(label, log_event_severity::stat);
    }

    auto configure(basic_config_intf& config) const -> bool {
        if(auto lbe{backend()}) {
            extract(lbe).configure(config);
        }
        return false;
    }

protected:
    constexpr basic_logger() noexcept = default;

    basic_logger(BackendGetter backend_getter) noexcept(
      std::is_nothrow_move_constructible_v<BackendGetter>)
      : BackendGetter(std::move(backend_getter)) {}

    void begin_log() noexcept {
        _backend_getter().begin_log();
    }

    void finish_log() noexcept {
        _backend_getter().finish_log();
    }

    void set_description(
      const identifier source,
      const string_view display_name,
      const string_view description) const noexcept {
        if(auto lbe{backend()}) {
            extract(lbe).set_description(
              source, instance_id(), display_name, description);
        }
    }

    void declare_state(
      const identifier source,
      const identifier state_tag,
      const identifier begin_tag,
      const identifier end_tag) const noexcept {
        if(auto lbe{backend()}) {
            extract(lbe).declare_state(source, state_tag, begin_tag, end_tag);
        }
    }

    void active_state(const identifier source, const identifier state_tag)
      const noexcept {
        if(auto lbe{backend()}) {
            extract(lbe).active_state(source, state_tag);
        }
    }

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
    constexpr auto make_log_entry(
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

    constexpr auto log_fatal(const identifier source, const string_view format)
      const noexcept {
        return make_log_entry(
          source,
          log_event_severity_constant<log_event_severity::fatal>{},
          format);
    }

    constexpr auto log_fatal(
      repeating_log_entry_control& control,
      const identifier source,
      const string_view format) const noexcept {
        return make_log_entry(
          control, source, log_event_severity::fatal, format);
    }

    constexpr auto log_error(const identifier source, const string_view format)
      const noexcept {
        return make_log_entry(
          source,
          log_event_severity_constant<log_event_severity::error>{},
          format);
    }

    constexpr auto log_error(
      repeating_log_entry_control& control,
      const identifier source,
      const string_view format) const noexcept {
        return make_log_entry(
          control, source, log_event_severity::error, format);
    }

    constexpr auto log_warning(
      const identifier source,
      const string_view format) const noexcept {
        return make_log_entry(
          source,
          log_event_severity_constant<log_event_severity::warning>{},
          format);
    }

    constexpr auto log_warning(
      repeating_log_entry_control& control,
      const identifier source,
      const string_view format) const noexcept {
        return make_log_entry(
          control, source, log_event_severity::warning, format);
    }

    constexpr auto log_info(const identifier source, const string_view format)
      const noexcept {
        return make_log_entry(
          source,
          log_event_severity_constant<log_event_severity::info>{},
          format);
    }

    constexpr auto log_stat(const identifier source, const string_view format)
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

    auto make_log_stream(
      const identifier source,
      const log_event_severity severity) const noexcept -> stream_log_entry {
        return {source, instance_id(), severity, _entry_backend(severity)};
    }

    void log_chart_sample(
      [[maybe_unused]] const identifier source,
      [[maybe_unused]] const identifier series,
      [[maybe_unused]] const float value) const noexcept {
        if constexpr(is_log_level_enabled_v<log_event_severity::stat>) {
            if(auto lbe{_entry_backend(log_event_severity::stat)}) {
                extract(lbe).log_chart_sample(
                  source, instance_id(), series, value);
            }
        }
    }

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
    auto _backend_getter() noexcept -> BackendGetter& {
        return *this;
    }
    auto _backend_getter() const noexcept -> const BackendGetter& {
        return *this;
    }
};
//------------------------------------------------------------------------------
/// @brief Basic template for logging objects.
/// @ingroup logging
/// @see logger
///
/// This class is used as the base class for other classes that need to do
/// logging. Prefer using this class instead of logger in such cases.
export template <typename BackendGetter>
class named_logging_object : public basic_logger<BackendGetter> {
    using base = basic_logger<BackendGetter>;

public:
    /// @brief The entry type for the specified log_event_severity.
    /// @see log_entry
    /// @see no_log_entry
    template <log_event_severity severity>
    using entry_type = std::
      conditional_t<is_log_level_enabled_v<severity>, log_entry, no_log_entry>;

    using base::log_lifetime;

    /// @brief Constructor from identifier and backend_getter object.
    /// @note For internal use-only.
    named_logging_object(
      const identifier id,
      BackendGetter backend_getter) noexcept
      : base{BackendGetter(std::move(backend_getter))}
      , _object_id{id} {}

    /// @brief Constructor from logger id and parent logging object.
    named_logging_object(
      const identifier id,
      const named_logging_object& parent) noexcept
      : base{static_cast<const base&>(parent)}
      , _object_id{id} {
        log_lifetime(_object_id, "created as a child of ${parent}")
          .tag("objCreate")
          .arg("parent", "LogId", parent._object_id);
    }

    /// @brief Construct logging object without backend.
    constexpr named_logging_object() noexcept = default;

    /// @brief Move constructor.
    constexpr named_logging_object(named_logging_object&& temp) noexcept
      : base{static_cast<base&&>(temp)}
      , _object_id{std::exchange(temp._object_id, identifier{})} {
        log_lifetime(_object_id, "being moved").tag("objMove");
    }

    /// @brief Copy constructor.
    constexpr named_logging_object(const named_logging_object& that) noexcept
      : base{static_cast<const base&>(that)}
      , _object_id{that._object_id} {
        log_lifetime(_object_id, "being copied").tag("objCopy");
    }

    /// @brief Move assignment operator.
    constexpr auto operator=(named_logging_object&& temp) noexcept
      -> named_logging_object& {
        if(this != &temp) {
            using std::swap;
            swap(static_cast<base&>(*this), static_cast<base&>(temp));
            swap(_object_id, temp._object_id);
        }
        return *this;
    }

    /// @brief Copy assignment operator.
    auto operator=(const named_logging_object& that) -> named_logging_object& {
        if(this != &that) {
            static_cast<base&>(*this) = static_cast<const base&>(that);
            _object_id = that._object_id;
        }
        return *this;
    }

    ~named_logging_object() noexcept {
        log_lifetime(_object_id, "being destroyed").tag("objDestroy");
    }

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

    /// @brief Returns a log entry stream for debug messages.
    /// @see log_event_severity
    /// @see stream_log_entry
    auto log_debug_stream() const noexcept {
        return make_log_stream(log_event_severity::debug);
    }

    /// @brief Returns a log entry stream for error messages.
    /// @see log_event_severity
    /// @see stream_log_entry
    auto log_error_stream() const noexcept {
        return make_log_stream(log_event_severity::error);
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
            base::log_chart_sample(
              _object_id, series, float(extract(opt_value)));
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

    auto make_log_stream(const log_event_severity severity) const noexcept {
        return base::make_log_stream(_object_id, severity);
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
export class logger
  : public named_logging_object<logger_shared_backend_getter> {
    using base = named_logging_object<logger_shared_backend_getter>;

public:
    /// @brief Default constructor.
    constexpr logger() noexcept = default;

    /// @brief Construction from identifier and backed getter
    logger(const identifier id, logger_shared_backend_getter getter) noexcept
      : base{id, std::move(getter)} {}

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

    /// @brief Returns a log entry stream for debug messages.
    /// @see log_event_severity
    /// @see stream_log_entry
    auto debug_stream() const noexcept {
        return log_debug_stream();
    }

    /// @brief Returns a log entry stream for error messages.
    /// @see log_event_severity
    /// @see stream_log_entry
    auto error_stream() const noexcept {
        return log_error_stream();
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
            base::log_chart_sample(series, float(extract(opt_value)));
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
};
//------------------------------------------------------------------------------
} // namespace eagine
