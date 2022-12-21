/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_XML_LOGS_INTERNAL_LOG_BACKEND_HPP
#define EAGINE_XML_LOGS_INTERNAL_LOG_BACKEND_HPP

import eagine.core;
#include "EntryStorage.hpp"
#include <cassert>

namespace eagine {
//------------------------------------------------------------------------------
class internal_log_backend final : public logger_backend {
public:
    internal_log_backend() noexcept {
        assert(not _single_instance_ptr());
        _single_instance_ptr() = this;
        LogStreamInfo info{};
        info.logIdentity = "XML log viewer";
        info.osName = extract_or(config_os_name(), string_view{});
        info.osCodeName = extract_or(config_os_code_name(), string_view{});
        info.gitBranch = extract_or(config_git_branch(), string_view{});
        info.gitHashId = extract_or(config_git_hash_id(), string_view{});
        info.gitVersion = extract_or(config_git_version(), string_view{});
        info.gitDescribe = extract_or(config_git_describe(), string_view{});
        info.architecture = extract_or(architecture_name(), string_view{});
        info.compilerName = extract_or(compiler_name(), string_view{});
        info.compilerVersionMajor = extract_or(compiler_version_major(), -1);
        info.compilerVersionMinor = extract_or(compiler_version_minor(), -1);
        info.compilerVersionPatch = extract_or(compiler_version_patch(), -1);
        _entries->beginStream(0, info);
    }

    internal_log_backend(internal_log_backend&&) = delete;
    internal_log_backend(const internal_log_backend&) = delete;
    auto operator=(internal_log_backend&&) = delete;
    auto operator=(const internal_log_backend&) = delete;

    ~internal_log_backend() noexcept final {
        _entries->endStream(0);
        assert(_single_instance_ptr());
        _single_instance_ptr() = nullptr;
    }

    static auto storage() noexcept -> std::shared_ptr<LogEntryStorage> {
        assert(_single_instance_ptr());
        return _single_instance_ptr()->_entries;
    }

private:
    static auto _single_instance_ptr() -> internal_log_backend*& {
        static internal_log_backend* the_instance = nullptr;
        return the_instance;
    }

    auto entry_backend(log_event_severity) noexcept -> logger_backend* final {
        return this;
    }

    auto allocator() noexcept -> memory::shared_byte_allocator final {
        return memory::default_byte_allocator();
    }

    auto type_id() noexcept -> identifier final {
        return "XmlLogView";
    }

    void begin_log() noexcept final {}

    void time_interval_begin(
      identifier,
      logger_instance_id,
      time_interval_id) noexcept final {}

    void time_interval_end(
      identifier,
      logger_instance_id,
      time_interval_id) noexcept final {}

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
        _current.streamId = 0U;
        _current.source = source;
        _current.tag = tag;
        _current.instance = instance;
        _current.severity = severity;
        _current.reltimeSec = std::chrono::duration<float>(
                                std::chrono::steady_clock::now() - _start)
                                .count();
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
        _entries->addEntry(std::move(_current));
    }

    void finish_log() noexcept final {}

    void log_chart_sample(
      identifier,
      logger_instance_id,
      identifier,
      float) noexcept final {}

    const std::chrono::steady_clock::time_point _start{
      std::chrono::steady_clock::now()};
    LogEntryData _current;
    std::shared_ptr<LogEntryStorage> _entries{
      std::make_shared<LogEntryStorage>()};
};
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_LOGGING_NULL_BACKEND_HPP
