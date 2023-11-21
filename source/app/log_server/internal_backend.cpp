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
void internal_backend::_set_sink(unique_holder<stream_sink> sink) noexcept {
    for(const auto& entry : _backlog) {
        std::visit([&](const auto& info) { sink->consume(info); }, entry);
    }
    _backlog.clear();
    _sink = std::move(sink);
}
//------------------------------------------------------------------------------
void internal_backend::set_sink(unique_holder<stream_sink> stream) noexcept {
    assert(_single_instance_ptr());
    _single_instance_ptr()->_set_sink(std::move(stream));
}
//------------------------------------------------------------------------------
void internal_backend::_dispatch(auto&& entry) noexcept {
    if(_sink) {
        _sink->consume(entry);
    } else {
        _backlog.emplace_back(decltype(entry)(entry));
    }
}
//------------------------------------------------------------------------------
auto internal_backend::_offset() const noexcept
  -> std::chrono::duration<float> {
    return std::chrono::duration_cast<std::chrono::duration<float>>(
      std::chrono::steady_clock::now() - _start);
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
void internal_backend::begin_log() noexcept {
    _dispatch(begin_info{});
}
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
  [[maybe_unused]] identifier source,
  [[maybe_unused]] logger_instance_id instance,
  [[maybe_unused]] string_view display_name,
  [[maybe_unused]] string_view description) noexcept {}
//------------------------------------------------------------------------------
void internal_backend::declare_state(
  [[maybe_unused]] const identifier source,
  [[maybe_unused]] const identifier state_tag,
  [[maybe_unused]] const identifier begin_tag,
  [[maybe_unused]] const identifier end_tag) noexcept {}
//------------------------------------------------------------------------------
void internal_backend::active_state(
  [[maybe_unused]] const identifier source,
  [[maybe_unused]] const identifier state_tag) noexcept {}
//------------------------------------------------------------------------------
auto internal_backend::begin_message(
  identifier source,
  identifier tag,
  logger_instance_id instance,
  log_event_severity severity,
  string_view format) noexcept -> bool {
    _dispatch(message_info{
      .offset = _offset(),
      .format = to_string(format),
      .severity = to_string(enumerator_name(severity)),
      .source = source.name().str(),
      .tag = tag.name().str(),
      .instance = instance});
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
void internal_backend::heartbeat() noexcept {
    _dispatch(heartbeat_info{.offset = _offset()});
}
//------------------------------------------------------------------------------
void internal_backend::finish_log() noexcept {
    _dispatch(finish_info{.offset = _offset()});
}
//------------------------------------------------------------------------------
void internal_backend::log_chart_sample(
  identifier,
  logger_instance_id,
  identifier,
  float) noexcept {}
//------------------------------------------------------------------------------
} // namespace eagine::logs
