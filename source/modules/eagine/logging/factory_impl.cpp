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
import :backend;

namespace eagine {
//------------------------------------------------------------------------------
auto make_asio_local_log_backend(
  string_view addr_str,
  const log_stream_info& info,
  const log_data_format format,
  const log_backend_lock lock_type) -> unique_holder<logger_backend> {
    auto output{make_asio_local_output_stream(addr_str)};
    switch(format) {
        case log_data_format::json:
            return make_json_log_backend(info, std::move(output), lock_type);
        case log_data_format::xml:
            return make_xml_log_backend(info, std::move(output), lock_type);
    }
}
//------------------------------------------------------------------------------
auto make_asio_tcpipv4_log_backend(
  string_view addr_str,
  const log_stream_info& info,
  const log_data_format format,
  const log_backend_lock lock_type) -> unique_holder<logger_backend> {
    auto output{make_asio_tcpipv4_output_stream(addr_str)};
    switch(format) {
        case log_data_format::json:
            return make_json_log_backend(info, std::move(output), lock_type);
        case log_data_format::xml:
            return make_xml_log_backend(info, std::move(output), lock_type);
    }
}
//------------------------------------------------------------------------------
auto make_ostream_log_backend(
  std::ostream& out,
  const log_stream_info& info,
  const log_data_format format,
  const log_backend_lock lock_type) -> unique_holder<logger_backend> {
    auto output{make_std_output_stream(out)};
    switch(format) {
        case log_data_format::json:
            return make_json_log_backend(info, std::move(output), lock_type);
        case log_data_format::xml:
            return make_xml_log_backend(info, std::move(output), lock_type);
    }
}
//------------------------------------------------------------------------------
} // namespace eagine
