/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_LOGGING_NULL_BACKEND_HPP
#define EAGINE_LOGGING_NULL_BACKEND_HPP

#include "backend.hpp"

namespace eagine {
//------------------------------------------------------------------------------
struct null_log_backend final : logger_backend {

    auto entry_backend(const identifier, const log_event_severity) noexcept
      -> logger_backend* final {
        return nullptr;
    }

    auto allocator() noexcept -> memory::shared_byte_allocator final {
        return {};
    }

    auto type_id() noexcept -> identifier final {
        return EAGINE_ID(Null);
    }

    void enter_scope(const identifier) noexcept final {}

    void leave_scope(const identifier) noexcept final {}

    void set_description(
      const identifier,
      const logger_instance_id,
      const string_view,
      const string_view) noexcept final {}

    auto begin_message(
      const identifier,
      const identifier,
      const logger_instance_id,
      const log_event_severity,
      const string_view) noexcept -> bool final {
        return false;
    }

    void add_nothing(const identifier, const identifier) noexcept final {}

    void add_identifier(
      const identifier,
      const identifier,
      const identifier) noexcept final {}

    void add_message_id(
      const identifier,
      const identifier,
      const message_id) noexcept final {}

    void add_bool(const identifier, const identifier, const bool) noexcept
      final {}

    void add_integer(
      const identifier,
      const identifier,
      const std::intmax_t) noexcept final {}

    void add_unsigned(
      const identifier,
      const identifier,
      const std::uintmax_t) noexcept final {}

    void add_float(const identifier, const identifier, const float) noexcept
      final {}

    void add_float(
      const identifier,
      const identifier,
      const float,
      const float,
      const float) noexcept final {}

    void add_duration(
      const identifier,
      const identifier,
      const std::chrono::duration<float>) noexcept final {}

    void add_string(
      const identifier,
      const identifier,
      const string_view) noexcept final {}

    void add_blob(
      const identifier,
      const identifier,
      const memory::const_block) noexcept final {}

    void finish_message() noexcept final {}

    void finish_log() noexcept final {}

    void log_chart_sample(
      const identifier,
      const logger_instance_id,
      const identifier,
      const float) noexcept final {}
};
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_LOGGING_NULL_BACKEND_HPP
