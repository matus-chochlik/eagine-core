/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.logging:time_interval;

import eagine.core.types;
import eagine.core.identifier;
import :backend;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Class representing a single log time interval measurement.
/// @ingroup logging
/// @note Do not use directly, use logger instead.
class log_time_interval {

public:
    auto interval_id() const noexcept {
        return reinterpret_cast<time_interval_id>(this);
    }

    log_time_interval(
      const identifier label,
      const logger_instance_id inst,
      logger_backend* backend) noexcept
      : _label{label}
      , _instance_id{inst}
      , _backend{backend} {
        if(_backend) {
            _backend->time_interval_begin(_label, _instance_id, interval_id());
        }
    }

    ~log_time_interval() noexcept {
        if(_backend) {
            _backend->time_interval_end(_label, _instance_id, interval_id());
        }
    }

private:
    const identifier _label;
    const logger_instance_id _instance_id;
    logger_backend* const _backend;
};
//------------------------------------------------------------------------------
} // namespace eagine
