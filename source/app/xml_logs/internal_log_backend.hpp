/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_XML_LOGS_INTERNAL_LOG_BACKEND_HPP
#define EAGINE_XML_LOGS_INTERNAL_LOG_BACKEND_HPP

#include "EntryStorage.hpp"
#include <eagine/assert.hpp>
#include <eagine/logging/backend.hpp>
#include <eagine/memory/default_alloc.hpp>

namespace eagine {
//------------------------------------------------------------------------------
class internal_log_backend final : public logger_backend {
public:
    internal_log_backend() noexcept {
        EAGINE_ASSERT(!_single_instance_ptr());
        _single_instance_ptr() = this;
    }

    internal_log_backend(internal_log_backend&&) = delete;
    internal_log_backend(const internal_log_backend&) = delete;
    auto operator=(internal_log_backend&&) = delete;
    auto operator=(const internal_log_backend&) = delete;

    ~internal_log_backend() noexcept final {
        EAGINE_ASSERT(_single_instance_ptr());
        _single_instance_ptr() = nullptr;
    }

    static auto storage() noexcept -> std::shared_ptr<LogEntryStorage> {
        EAGINE_ASSERT(_single_instance_ptr());
        return _single_instance_ptr()->_entries;
    }

private:
    static auto _single_instance_ptr() -> internal_log_backend*& {
        static internal_log_backend* the_instance = nullptr;
        return the_instance;
    }

    auto entry_backend(identifier, log_event_severity) noexcept
      -> logger_backend* final {
        return this;
    }

    auto allocator() noexcept -> memory::shared_byte_allocator final {
        return memory::default_byte_allocator();
    }

    auto type_id() noexcept -> identifier final {
        return EAGINE_ID(XmlLogView);
    }

    void enter_scope(identifier) noexcept final {}

    void leave_scope(identifier) noexcept final {}

    void set_description(
      identifier source,
      logger_instance_id instance,
      string_view display_name,
      string_view description) noexcept final {
        _entries->setDescription(
          0U, source, instance, display_name, description);
    }

    auto begin_message(
      identifier source,
      identifier tag,
      logger_instance_id instance,
      log_event_severity severity,
      string_view format) noexcept -> bool final {
        _current.stream_id = 0U;
        _current.source = source;
        _current.tag = tag;
        _current.instance = instance;
        _current.severity = severity;
        _current.format = _entries->cacheString(format);
        return true;
    }

    void add_nothing(identifier name, identifier tag) noexcept final {
        _current.args[name] = {tag, nothing};
    }

    void add_identifier(
      identifier name,
      identifier tag,
      identifier value) noexcept final {
        _current.args[name] = {tag, value};
    }

    void add_message_id(
      identifier name,
      identifier tag,
      message_id value) noexcept final {
        _current.args[name] = {tag, value};
    }

    void add_bool(identifier name, identifier tag, bool value) noexcept final {
        _current.args[name] = {tag, value};
    }

    void add_integer(
      identifier name,
      identifier tag,
      std::intmax_t value) noexcept final {
        _current.args[name] = {tag, value};
    }

    void add_unsigned(
      identifier name,
      identifier tag,
      std::uintmax_t value) noexcept final {
        _current.args[name] = {tag, value};
    }

    void add_float(identifier name, identifier tag, float value) noexcept final {
        _current.args[name] = {tag, value};
    }

    void add_float(
      identifier name,
      identifier tag,
      float min,
      float value,
      float max) noexcept final {
        _current.args[name] = {tag, std::make_tuple(min, value, max)};
    }

    void add_duration(
      identifier name,
      identifier tag,
      std::chrono::duration<float> value) noexcept final {
        _current.args[name] = {tag, value};
    }

    void add_string(identifier name, identifier tag, string_view value) noexcept
      final {
        _current.args[name] = {tag, _entries->cacheString(value)};
    }

    void add_blob(identifier, identifier, memory::const_block) noexcept final {}

    void finish_message() noexcept final {
        _entries->addEntry(_current);
    }

    void finish_log() noexcept final {}

    void log_chart_sample(
      identifier,
      logger_instance_id,
      identifier,
      float) noexcept final {}

    LogEntryData _current;
    std::shared_ptr<LogEntryStorage> _entries{
      std::make_shared<LogEntryStorage>()};
};
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_LOGGING_NULL_BACKEND_HPP
