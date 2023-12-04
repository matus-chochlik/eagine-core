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
import :backend;

namespace eagine {
//------------------------------------------------------------------------------
template <typename Lockable>
auto make_ostream_log_backend(
  const log_stream_info& info,
  const log_data_format format) -> unique_holder<logger_backend> {
    switch(format) {
        case log_data_format::xml:
            return {
              hold<ostream_log_backend<spinlock, log_data_format::xml>>,
              std::cerr,
              info};
        case log_data_format::json:
            return {
              hold<ostream_log_backend<spinlock, log_data_format::json>>,
              std::cerr,
              info};
    }
}
//------------------------------------------------------------------------------
auto make_ostream_log_backend(
  const log_stream_info& info,
  const log_data_format format,
  const bool use_spinlock) -> unique_holder<logger_backend> {
    if(use_spinlock) {
        return make_ostream_log_backend<spinlock>(info, format);
    } else {
        return make_ostream_log_backend<std::mutex>(info, format);
    }
}
//------------------------------------------------------------------------------
} // namespace eagine
