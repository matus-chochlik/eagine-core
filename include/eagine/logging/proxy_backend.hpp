/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_LOGGING_PROXY_BACKEND_HPP
#define EAGINE_LOGGING_PROXY_BACKEND_HPP

#include "backend.hpp"
#include <functional>
#include <vector>

namespace eagine {
//------------------------------------------------------------------------------
class proxy_log_backend : public logger_backend {
public:
    proxy_log_backend(const log_event_severity min_severity) noexcept
      : _min_severity{min_severity} {}

    auto configure(application_config&) -> bool final;

    auto entry_backend(
      const identifier source,
      const log_event_severity severity) noexcept -> logger_backend* final;

    auto allocator() noexcept -> memory::shared_byte_allocator final;
    auto type_id() noexcept -> identifier final;

    void enter_scope(const identifier source) noexcept final;
    void leave_scope(const identifier source) noexcept final;

    void set_description(
      const identifier source,
      const logger_instance_id instance,
      const string_view name,
      const string_view desc) noexcept final;

    auto begin_message(
      const identifier source,
      const identifier tag,
      const logger_instance_id instance,
      const log_event_severity severity,
      const string_view format) noexcept -> bool final;

    void add_nothing(const identifier arg, const identifier tag) noexcept final;

    void add_identifier(
      const identifier arg,
      const identifier tag,
      const identifier value) noexcept final;

    void add_message_id(
      const identifier arg,
      const identifier tag,
      const message_id value) noexcept final;

    void add_bool(
      const identifier arg,
      const identifier tag,
      const bool value) noexcept final;

    void add_integer(
      const identifier arg,
      const identifier tag,
      const std::intmax_t value) noexcept final;

    void add_unsigned(
      const identifier arg,
      const identifier tag,
      const std::uintmax_t value) noexcept final;

    void add_float(
      const identifier arg,
      const identifier tag,
      const float value) noexcept final;

    void add_float(
      const identifier arg,
      const identifier tag,
      const float min,
      const float value,
      const float max) noexcept final;

    void add_duration(
      const identifier arg,
      const identifier tag,
      const std::chrono::duration<float> value) noexcept final;

    void add_string(
      const identifier arg,
      const identifier tag,
      const string_view value) noexcept final;

    void add_blob(
      const identifier arg,
      const identifier tag,
      const memory::const_block value) noexcept final;

    void finish_message() noexcept final;

    void finish_log() noexcept final;

    void log_chart_sample(
      const identifier source,
      const logger_instance_id instance,
      const identifier series,
      const float value) noexcept final;

private:
    std::unique_ptr<logger_backend> _delegate;
    std::unique_ptr<std::vector<std::function<void()>>> _delayed{
      std::make_unique<std::vector<std::function<void()>>>()};
    log_event_severity _min_severity;
};
//------------------------------------------------------------------------------
} // namespace eagine

#if !EAGINE_CORE_LIBRARY || defined(EAGINE_IMPLEMENTING_CORE_LIBRARY)
#include <eagine/logging/proxy_backend.inl>
#endif

#endif // EAGINE_LOGGING_PROXY_BACKEND_HPP
