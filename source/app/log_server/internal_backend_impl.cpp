/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

module eagine.core.log_server;
import std;
import eagine.core;

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
auto internal_backend::_offset() const noexcept -> float_seconds {
    return std::chrono::duration_cast<float_seconds>(
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
    _message.offset = _offset();
    _message.format = to_string(format);
    _message.severity = to_string(enumerator_name(severity));
    _message.source = source.name().str();
    _message.tag = tag.name().str();
    _message.instance = instance;
    _message.args.clear();
    return true;
}
//------------------------------------------------------------------------------
void internal_backend::add_nothing(
  [[maybe_unused]] identifier name,
  [[maybe_unused]] identifier tag) noexcept {}
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
    _message.args.emplace_back(
      message_info::arg_info{.name = name, .tag = tag, .value = value});
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
    _message.args.emplace_back(
      message_info::arg_info{.name = name, .tag = tag, .value = value});
}
//------------------------------------------------------------------------------
void internal_backend::add_float(
  identifier name,
  identifier tag,
  float min,
  float value,
  float max) noexcept {
    _message.args.emplace_back(message_info::arg_info{
      .name = name, .tag = tag, .value = value, .min = min, .max = max});
}
//------------------------------------------------------------------------------
void internal_backend::add_duration(
  identifier name,
  identifier tag,
  float_seconds value) noexcept {
    _message.args.emplace_back(
      message_info::arg_info{.name = name, .tag = tag, .value = value});
}
//------------------------------------------------------------------------------
void internal_backend::add_string(
  identifier name,
  identifier tag,
  string_view value) noexcept {
    _message.args.emplace_back(message_info::arg_info{
      .name = name, .tag = tag, .value = to_string(value)});
}
//------------------------------------------------------------------------------
void internal_backend::add_blob(
  identifier,
  identifier,
  memory::const_block) noexcept {}
//------------------------------------------------------------------------------
void internal_backend::finish_message() noexcept {
    _dispatch(_message);
}
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
