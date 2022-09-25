/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

module eagine.core.logging;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.identifier;
import eagine.core.reflection;
import eagine.core.build_info;
import eagine.core.valid_if;
import eagine.core.utility;
import eagine.core.runtime;
import :config;
import :severity;
import :entry;
import :backend;
import :null_backend;
import :ostream_backend;
import :syslog_backend;
import :proxy_backend;
import <cerrno>;
import <iostream>;
import <optional>;
import <mutex>;

namespace eagine {
//------------------------------------------------------------------------------
static constexpr auto default_log_severity() noexcept {
    return static_cast<log_event_severity>(
      static_cast<std::underlying_type_t<log_event_severity>>(
        min_log_severity) +
      1);
}
//------------------------------------------------------------------------------
auto root_logger_choose_backend(
  const program_args& args,
  const root_logger_options& opts,
  const log_stream_info& info) -> std::unique_ptr<logger_backend> {
    std::unique_ptr<logger_backend> result{};

    const bool use_spinlock{args.find("--log-use-spinlock")};

    for(auto& arg : args) {
        if(arg.is_long_tag("use-null-log")) {
            return make_null_log_backend();
        } else if(arg.is_long_tag("use-cerr-log")) {
            if(use_spinlock) {
                return std::make_unique<ostream_log_backend<spinlock>>(
                  std::cerr, info);
            } else {
                return std::make_unique<ostream_log_backend<std::mutex>>(
                  std::cerr, info);
            }
        } else if(arg.is_long_tag("use-cout-log")) {
            if(use_spinlock) {
                return std::make_unique<ostream_log_backend<spinlock>>(
                  std::cout, info);
            } else {
                return std::make_unique<ostream_log_backend<std::mutex>>(
                  std::cout, info);
            }
        } else if(arg.is_long_tag("use-syslog")) {
            if(use_spinlock) {
                return std::make_unique<syslog_log_backend<spinlock>>(info);
            } else {
                return std::make_unique<syslog_log_backend<std::mutex>>(info);
            }
        } else if(arg.is_long_tag("use-asio-nw-log")) {
            string_view nw_addr;
            if(arg.next() && !arg.next().starts_with("-")) {
                nw_addr = arg.next();
            } else if(const auto env_var{get_environment_variable(
                        "EAGINE_LOG_NETWORK_ADDRESS")}) {
                nw_addr = extract(env_var);
            }
            if(use_spinlock) {
                return make_asio_tcpipv4_ostream_log_backend_spinlock(
                  nw_addr, info);
            } else {
                return make_asio_tcpipv4_ostream_log_backend_mutex(
                  nw_addr, info);
            }
        } else if(arg.is_long_tag("use-asio-log")) {
            if(use_spinlock) {
                return make_asio_local_ostream_log_backend_spinlock(info);
            } else {
                return make_asio_local_ostream_log_backend_mutex(info);
            }
        }
    }

    if(opts.default_no_log) {
        return make_null_log_backend();
    }

    return make_proxy_log_backend(info);
}
//------------------------------------------------------------------------------
auto root_logger_init_backend(
  const program_args& args,
  root_logger_options& opts) -> std::unique_ptr<logger_backend> {
    if(opts.forced_backend) {
        return std::move(opts.forced_backend);
    }

    log_stream_info info{};
    info.min_severity = default_log_severity();

    for(auto arg = args.first(); arg; arg = arg.next()) {
        if(arg.is_long_tag("min-log-severity")) {
            if(arg.next().parse(info.min_severity, std::cerr)) {
                arg = arg.next();
            }
        } else if(arg.is_long_tag("log-identity")) {
            if(arg.next().parse(info.log_identity, std::cerr)) {
                arg = arg.next();
            }
        }
    }

    auto backend = root_logger_choose_backend(args, opts, info);

    return backend;
}
//------------------------------------------------------------------------------
auto root_logger::_log_args(const program_args& args) -> void {
    auto args_entry{info("program arguments:")};
    args_entry.tag("ProgArgs");
    args_entry.arg("cmd", args.command().get());
    for(const auto& arg : args) {
        args_entry.arg("arg", arg.get());
    }
}
//------------------------------------------------------------------------------
auto root_logger::_log_os_info() -> void {
    const string_view not_available{"N/A"};
    info("build OS information")
      .tag("OSInfo")
      .arg("osName", "OSName", config_os_name(), not_available)
      .arg("osCodeName", "OSCodeName", config_os_code_name(), not_available);
}
//------------------------------------------------------------------------------
auto root_logger::_log_git_info() -> void {
    const string_view not_available{"N/A"};
    info("source version information")
      .tag("GitInfo")
      .arg("gitBranch", "GitBranch", config_git_branch(), not_available)
      .arg("gitHashId", "GitHash", config_git_hash_id(), not_available)
      .arg("gitDate", "RFC2822", config_git_date(), not_available)
      .arg("gitDescrib", "str", config_git_describe(), not_available)
      .arg("gitVersion", "str", config_git_version(), not_available);
}
//------------------------------------------------------------------------------
auto root_logger::_log_instance_info() -> void {
    info("instance information")
      .tag("Instance")
      .arg("instanceId", process_instance_id());
}
//------------------------------------------------------------------------------
auto root_logger::_log_compiler_info() -> void {
    info("built with ${complrName} compiler for ${archtcture} architecture")
      .tag("Compiler")
      .arg(
        "complrName", "string", compiler_name(), string_view{"not_available"})
      .arg(
        "archtcture",
        "string",
        architecture_name(),
        string_view{"not_available"})
      .arg_func([](logger_backend& backend) {
          if(const auto version_major{compiler_version_major()}) {
              backend.add_integer(
                "complrMajr", "VrsnMajor", extract(version_major));
          }
          if(const auto version_minor{compiler_version_minor()}) {
              backend.add_integer(
                "complrMinr", "VrsnMinor", extract(version_minor));
          }
          if(const auto version_patch{compiler_version_patch()}) {
              backend.add_integer(
                "complrPtch", "VrsnPatch", extract(version_patch));
          }
      });
}
//------------------------------------------------------------------------------
root_logger::root_logger(
  identifier logger_id,
  const program_args& args,
  root_logger_options& opts) noexcept
  : logger{logger_id, {root_logger_init_backend(args, opts)}} {
    _log_args(args);
    _log_instance_info();
    _log_git_info();
    _log_os_info();
    _log_compiler_info();
}
//------------------------------------------------------------------------------
} // namespace eagine
