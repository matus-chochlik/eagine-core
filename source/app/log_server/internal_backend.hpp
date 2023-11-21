/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#ifndef EAGINE_CORE_LOG_SERVER_INTERNAL_BACKEND_HPP
#define EAGINE_CORE_LOG_SERVER_INTERNAL_BACKEND_HPP

#include "interfaces.hpp"

namespace eagine::logs {
//------------------------------------------------------------------------------
class internal_backend final : public logger_backend {
public:
    internal_backend() noexcept;
    internal_backend(internal_backend&&) = delete;
    internal_backend(const internal_backend&) = delete;
    auto operator=(internal_backend&&) = delete;
    auto operator=(const internal_backend&) = delete;
    ~internal_backend() noexcept final;

private:
    static auto _single_instance_ptr() -> internal_backend*&;

    auto entry_backend(log_event_severity) noexcept -> logger_backend* final;

    auto allocator() noexcept -> memory::shared_byte_allocator final;

    auto type_id() noexcept -> identifier final;

    void begin_log() noexcept final;

    void time_interval_begin(
      identifier,
      logger_instance_id,
      time_interval_id) noexcept final;

    void time_interval_end(
      identifier,
      logger_instance_id,
      time_interval_id) noexcept final;

    void set_description(
      identifier source,
      logger_instance_id instance,
      string_view display_name,
      string_view description) noexcept final;

    void declare_state(
      const identifier source,
      const identifier state_tag,
      const identifier begin_tag,
      const identifier end_tag) noexcept final;

    void active_state(
      const identifier source,
      const identifier state_tag) noexcept final;

    auto begin_message(
      identifier source,
      identifier tag,
      logger_instance_id instance,
      log_event_severity severity,
      string_view format) noexcept -> bool final;

    void add_nothing(identifier name, identifier tag) noexcept final;

    void add_identifier(
      identifier name,
      identifier tag,
      identifier value) noexcept final;

    void add_message_id(
      identifier name,
      identifier tag,
      message_id value) noexcept final;

    void add_bool(identifier name, identifier tag, bool value) noexcept final;

    void add_integer(
      identifier name,
      identifier tag,
      std::intmax_t value) noexcept final;

    void add_unsigned(
      identifier name,
      identifier tag,
      std::uintmax_t value) noexcept final;

    void add_float(identifier name, identifier tag, float value) noexcept final;

    void add_float(
      identifier name,
      identifier tag,
      float min,
      float value,
      float max) noexcept final;

    void add_duration(
      identifier name,
      identifier tag,
      std::chrono::duration<float> value) noexcept final;

    void add_string(identifier name, identifier tag, string_view value) noexcept
      final;

    void add_blob(identifier, identifier, memory::const_block) noexcept final;

    void finish_message() noexcept final;

    void heartbeat() noexcept final;

    void finish_log() noexcept final;

    void log_chart_sample(
      identifier,
      logger_instance_id,
      identifier,
      float) noexcept final;
};
//------------------------------------------------------------------------------
} // namespace eagine::logs

#endif
