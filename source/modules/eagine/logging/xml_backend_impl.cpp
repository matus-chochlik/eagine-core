/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.logging;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.identifier;
import eagine.core.reflection;
import eagine.core.utility;
import :backend;

namespace eagine {
//------------------------------------------------------------------------------
// factory
//------------------------------------------------------------------------------
auto make_xml_log_backend(
  const log_stream_info& info,
  shared_holder<log_output_stream> output,
  const log_backend_lock lock_type) -> unique_holder<logger_backend> {
    (void)info;
    (void)output;
    // TODO
    switch(lock_type) {
        case log_backend_lock::none:
            return {};
        case log_backend_lock::mutex:
            return {};
        case log_backend_lock::spinlock:
            return {};
    }
}
//------------------------------------------------------------------------------
} // namespace eagine
