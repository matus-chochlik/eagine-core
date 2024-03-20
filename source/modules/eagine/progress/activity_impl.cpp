/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.progress;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.utility;
import eagine.core.runtime;

namespace eagine {
//------------------------------------------------------------------------------
auto activity_progress::backend() const noexcept
  -> optional_reference<progress_tracker_backend> {
    return _backend;
}
//------------------------------------------------------------------------------
activity_progress::activity_progress(
  const activity_progress& parent,
  const string_view title,
  const span_size_t total_steps) noexcept
  : _backend{parent._backend}
  , _activity_id{
      _backend
        ? _backend->begin_activity(parent._activity_id, title, total_steps)
        : 0U} {}
//------------------------------------------------------------------------------
activity_progress::~activity_progress() noexcept {
    finish();
}
//------------------------------------------------------------------------------
auto activity_progress::activity(
  const string_view title,
  const span_size_t total_steps) const noexcept -> activity_progress {
    return {*this, title, total_steps};
}
//------------------------------------------------------------------------------
auto activity_progress::configure(basic_config_intf& config) const -> bool {
    if(auto pbe{backend()}) {
        return pbe->configure(config);
    }
    return false;
}
//------------------------------------------------------------------------------
auto activity_progress::update_progress(span_size_t current) const noexcept
  -> bool {
    if(auto pbe{backend()}) {
        return pbe->update_progress(_activity_id, current);
    }
    return true;
}
//------------------------------------------------------------------------------
auto activity_progress::advance_progress(span_size_t increment) const noexcept
  -> bool {
    if(auto pbe{backend()}) {
        return pbe->advance_progress(_activity_id, increment);
    }
    return true;
}
//------------------------------------------------------------------------------
void activity_progress::finish() noexcept {
    if(_backend) {
        _backend->finish_activity(_activity_id);
        _backend.reset();
    }
}
//------------------------------------------------------------------------------
} // namespace eagine
