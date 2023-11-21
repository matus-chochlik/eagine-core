/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include "internal_backend.hpp"
#include <cassert>

namespace eagine::logs {
//------------------------------------------------------------------------------
auto internal_backend::_single_instance_ptr() -> internal_backend*& {
    static internal_backend* the_instance{nullptr};
    return the_instance;
}
//------------------------------------------------------------------------------
internal_backend::internal_backend() noexcept {
    assert(not _single_instance_ptr());
    _single_instance_ptr() = this;
}
//------------------------------------------------------------------------------
internal_backend::~internal_backend() noexcept {
    assert(_single_instance_ptr());
    _single_instance_ptr() = nullptr;
}
//------------------------------------------------------------------------------
auto internal_backend::entry_backend(log_event_severity) noexcept
  -> logger_backend* {
    return this;
}
//------------------------------------------------------------------------------
auto internal_backend::allocator() noexcept -> memory::shared_byte_allocator {
    return memory::default_byte_allocator();
}
//------------------------------------------------------------------------------
auto internal_backend::type_id() noexcept -> identifier {
    return "LogServer";
}
//------------------------------------------------------------------------------
void internal_backend::begin_log() noexcept {}
//------------------------------------------------------------------------------
void internal_backend::time_interval_begin(
  identifier,
  logger_instance_id,
  time_interval_id) noexcept {}
//------------------------------------------------------------------------------
void internal_backend::time_interval_end(
  identifier,
  logger_instance_id,
  time_interval_id) noexcept {}
//------------------------------------------------------------------------------
void internal_backend::set_description(
  identifier source,
  logger_instance_id instance,
  string_view display_name,
  string_view description) noexcept {
    (void)source;
    (void)instance;
    (void)display_name;
    (void)description;
}
//------------------------------------------------------------------------------
void internal_backend::declare_state(
  const identifier source,
  const identifier state_tag,
  const identifier begin_tag,
  const identifier end_tag) noexcept {
    (void)source;
    (void)state_tag;
    (void)begin_tag;
    (void)end_tag;
}
//------------------------------------------------------------------------------
void internal_backend::active_state(
  const identifier source,
  const identifier state_tag) noexcept {
    (void)source;
    (void)state_tag;
}
//------------------------------------------------------------------------------
auto internal_backend::begin_message(
  identifier source,
  identifier tag,
  logger_instance_id instance,
  log_event_severity severity,
  string_view format) noexcept -> bool {
    (void)source;
    (void)tag;
    (void)instance;
    (void)severity;
    (void)format;
    return true;
}
//------------------------------------------------------------------------------
void internal_backend::add_nothing(identifier name, identifier tag) noexcept {
    (void)name;
    (void)tag;
}
//------------------------------------------------------------------------------
void internal_backend::add_identifier(
  identifier name,
  identifier tag,
  identifier value) noexcept {
    (void)name;
    (void)tag;
    (void)value;
}
//------------------------------------------------------------------------------
void internal_backend::add_message_id(
  identifier name,
  identifier tag,
  message_id value) noexcept {
    (void)name;
    (void)tag;
    (void)value;
}
//------------------------------------------------------------------------------
void internal_backend::add_bool(
  identifier name,
  identifier tag,
  bool value) noexcept {
    (void)name;
    (void)tag;
    (void)value;
}
//------------------------------------------------------------------------------
void internal_backend::add_integer(
  identifier name,
  identifier tag,
  std::intmax_t value) noexcept {
    (void)name;
    (void)tag;
    (void)value;
}
//------------------------------------------------------------------------------
void internal_backend::add_unsigned(
  identifier name,
  identifier tag,
  std::uintmax_t value) noexcept {
    (void)name;
    (void)tag;
    (void)value;
}
//------------------------------------------------------------------------------
void internal_backend::add_float(
  identifier name,
  identifier tag,
  float value) noexcept {
    (void)name;
    (void)tag;
    (void)value;
}
//------------------------------------------------------------------------------
void internal_backend::add_float(
  identifier name,
  identifier tag,
  float min,
  float value,
  float max) noexcept {
    (void)name;
    (void)tag;
    (void)min;
    (void)max;
    (void)value;
}
//------------------------------------------------------------------------------
void internal_backend::add_duration(
  identifier name,
  identifier tag,
  std::chrono::duration<float> value) noexcept {
    (void)name;
    (void)tag;
    (void)value;
}
//------------------------------------------------------------------------------
void internal_backend::add_string(
  identifier name,
  identifier tag,
  string_view value) noexcept {
    (void)name;
    (void)tag;
    (void)value;
}
//------------------------------------------------------------------------------
void internal_backend::add_blob(
  identifier,
  identifier,
  memory::const_block) noexcept {}
//------------------------------------------------------------------------------
void internal_backend::finish_message() noexcept {}
//------------------------------------------------------------------------------
void internal_backend::heartbeat() noexcept {}
//------------------------------------------------------------------------------
void internal_backend::finish_log() noexcept {}
//------------------------------------------------------------------------------
void internal_backend::log_chart_sample(
  identifier,
  logger_instance_id,
  identifier,
  float) noexcept {}
//------------------------------------------------------------------------------
} // namespace eagine::logs
