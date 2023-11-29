/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.logging;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.identifier;
import eagine.core.utility;
import eagine.core.runtime;

namespace eagine {
//------------------------------------------------------------------------------
// basic logger
//------------------------------------------------------------------------------
void basic_logger::begin_log() noexcept {
    if(auto lbe{backend()}) {
        lbe->begin_log();
    }
}
//------------------------------------------------------------------------------
void basic_logger::finish_log() noexcept {
    if(auto lbe{backend()}) {
        lbe->begin_log();
    }
}
//------------------------------------------------------------------------------
void basic_logger::heartbeat() const noexcept {
    if(auto lbe{backend()}) {
        lbe->heartbeat();
    }
}
//------------------------------------------------------------------------------
auto basic_logger::register_time_interval(const identifier label) const noexcept
  -> time_interval_handle {
    if(auto lbe{backend()}) {
        return {._id = lbe->register_time_interval(label, instance_id())};
    }
    return {._id = 0U};
}
//------------------------------------------------------------------------------
auto basic_logger::configure(basic_config_intf& config) const -> bool {
    if(auto lbe{backend()}) {
        lbe->configure(config);
    }
    return false;
}
//------------------------------------------------------------------------------
void basic_logger::set_description(
  const identifier source,
  const string_view display_name,
  const string_view description) const noexcept {
    if(auto lbe{backend()}) {
        lbe->set_description(source, instance_id(), display_name, description);
    }
}
//------------------------------------------------------------------------------
void basic_logger::declare_state(
  const identifier source,
  const identifier state_tag,
  const identifier begin_tag,
  const identifier end_tag) const noexcept {
    if(auto lbe{backend()}) {
        lbe->declare_state(source, state_tag, begin_tag, end_tag);
    }
}
//------------------------------------------------------------------------------
void basic_logger::active_state(
  const identifier source,
  const identifier state_tag) const noexcept {
    if(auto lbe{backend()}) {
        lbe->active_state(source, state_tag);
    }
}
//------------------------------------------------------------------------------
void basic_logger::log_chart_sample(
  [[maybe_unused]] const identifier source,
  [[maybe_unused]] const identifier series,
  [[maybe_unused]] const float value) const noexcept {
    if constexpr(is_log_level_enabled_v<log_event_severity::stat>) {
        if(auto lbe{_entry_backend(log_event_severity::stat)}) {
            lbe->log_chart_sample(source, instance_id(), series, value);
        }
    }
}
//------------------------------------------------------------------------------
// named logging object
//------------------------------------------------------------------------------
named_logging_object::named_logging_object(
  const identifier id,
  const named_logging_object& parent) noexcept
  : base{static_cast<const base&>(parent)}
  , _object_id{id} {
    log_lifetime(_object_id, "created as a child of ${parent}")
      .tag("objCreate")
      .arg("parent", "LogId", parent._object_id);
}
//------------------------------------------------------------------------------
named_logging_object::named_logging_object(named_logging_object&& temp) noexcept
  : base{static_cast<base&&>(temp)}
  , _object_id{std::exchange(temp._object_id, identifier{})} {
    log_lifetime(_object_id, "being moved").tag("objMove");
}
//------------------------------------------------------------------------------
named_logging_object::named_logging_object(
  const named_logging_object& that) noexcept
  : base{static_cast<const base&>(that)}
  , _object_id{that._object_id} {
    log_lifetime(_object_id, "being copied").tag("objCopy");
}
//------------------------------------------------------------------------------
auto named_logging_object::operator=(named_logging_object&& temp) noexcept
  -> named_logging_object& {
    if(this != &temp) {
        using std::swap;
        swap(static_cast<base&>(*this), static_cast<base&>(temp));
        swap(_object_id, temp._object_id);
    }
    return *this;
}
//------------------------------------------------------------------------------
auto named_logging_object::operator=(const named_logging_object& that)
  -> named_logging_object& {
    if(this != &that) {
        static_cast<base&>(*this) = static_cast<const base&>(that);
        _object_id = that._object_id;
    }
    return *this;
}
//------------------------------------------------------------------------------
named_logging_object::~named_logging_object() noexcept {
    if(_object_id) {
        log_lifetime(_object_id, "being destroyed").tag("objDestroy");
    }
}
//------------------------------------------------------------------------------
// logger
//------------------------------------------------------------------------------
[[nodiscard]] auto logger::log_when_switched(signal_switch& s) const noexcept
  -> signal_binding {
    return s.switched.bind(
      {this, member_function_constant_t<&logger::_handle_interrupt_signal>{}});
}
//------------------------------------------------------------------------------
void logger::_handle_interrupt_signal(bool is_signaled) const noexcept {
    if(is_signaled) {
        log_info("process interrupted by signal").tag("interrupt");
    }
}
//------------------------------------------------------------------------------
} // namespace eagine
