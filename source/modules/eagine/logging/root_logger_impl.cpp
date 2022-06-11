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
import eagine.core.utility;
import eagine.core.identifier;
import eagine.core.build_info;
import eagine.core.valid_if;
import :config;
import :severity;
import :entry;
import :null_backend;
import :ostream_backend;
import :syslog_backend;
import :proxy_backend;
import <cerrno>;
import <iostream>;
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

    for(auto& arg : args) {
        if(arg.is_long_tag("use-null-log")) {
            return make_null_log_backend();
        } else if(arg.is_long_tag("use-cerr-log")) {
            return std::make_unique<ostream_log_backend<std::mutex>>(
              std::cerr, info);
        } else if(arg.is_long_tag("use-cout-log")) {
            return std::make_unique<ostream_log_backend<std::mutex>>(
              std::cout, info);
        } else if(arg.is_long_tag("use-syslog")) {
            return std::make_unique<syslog_log_backend<std::mutex>>(info);
            /* TODO
                                } else if(arg.is_long_tag("use-asio-nw-log")) {
                                    string_view nw_addr;
                                    if(arg.next() &&
               !arg.next().starts_with("-")) { nw_addr = arg.next(); } else
               if(const auto env_var{get_environment_variable(
               "EAGINE_LOG_NETWORK_ADDRESS")}) { nw_addr = extract(env_var);
                                    }
                                    return
                        std::make_unique<asio_tcpipv4_ostream_log_backend<>>(
               nw_addr, info); #if EAGINE_HAS_ASIO_LOCAL_LOG_BACKEND } else
                        if(arg.is_long_tag("use-asio-log")) { return
                        std::make_unique<asio_local_ostream_log_backend<>>(info);
               #endif
                        */
        }
    }

    if(opts.default_no_log) {
        return make_null_log_backend();
    }

    return make_proxy_log_backend(info);
}
//------------------------------------------------------------------------------
auto root_logger::_init_backend(
  const program_args& args,
  root_logger_options& opts) -> std::unique_ptr<logger_backend> {
    if(opts.forced_backend) {
        return std::move(opts.forced_backend);
    }

    log_stream_info info{};
    info.min_severity = default_log_severity();

    for(auto arg = args.first(); arg; arg = arg.next()) {
        if(arg.is_long_tag("min-log-severity")) {
            /* TODO
            if(arg.next().parse(info.min_severity, std::cerr)) {
                arg = arg.next();
            }
            */
        } else if(arg.is_long_tag("log-identity")) {
            /* TODO
            if(arg.next().parse(info.log_identity, std::cerr)) {
                arg = arg.next();
            }
            */
        }
    }

    auto backend = root_logger_choose_backend(args, opts, info);

    return backend;
}
//------------------------------------------------------------------------------
auto root_logger::_log_args(const program_args& args) -> void {
    auto args_entry{info("program arguments:")};
    args_entry.tag(identifier{"ProgArgs"});
    args_entry.arg(identifier{"cmd"}, args.command().get());
    for(const auto& arg : args) {
        args_entry.arg(identifier{"arg"}, arg.get());
    }
}
//------------------------------------------------------------------------------
auto root_logger::_log_os_info() -> void {
    const string_view not_available{"N/A"};
    info("build OS information")
      .tag(identifier{"OSInfo"})
      .arg(
        identifier{"osName"},
        identifier{"OSName"},
        config_os_name(),
        not_available)
      .arg(
        identifier{"osCodeName"},
        identifier{"OSCodeName"},
        config_os_code_name(),
        not_available);
}
//------------------------------------------------------------------------------
auto root_logger::_log_git_info() -> void {
    const string_view not_available{"N/A"};
    info("source version information")
      .tag(identifier{"GitInfo"})
      .arg(
        identifier{"gitBranch"},
        identifier{"GitBranch"},
        config_git_branch(),
        not_available)
      .arg(
        identifier{"gitHashId"},
        identifier{"GitHash"},
        config_git_hash_id(),
        not_available)
      .arg(
        identifier{"gitDate"},
        identifier{"RFC2822"},
        config_git_date(),
        not_available)
      .arg(
        identifier{"gitDescrib"},
        identifier{"str"},
        config_git_describe(),
        not_available)
      .arg(
        identifier{"gitVersion"},
        identifier{"str"},
        config_git_version(),
        not_available);
}
//------------------------------------------------------------------------------
auto root_logger::_log_instance_info() -> void {
    info("instance information")
      .tag(identifier{"Instance"})
      .arg(identifier{"instanceId"}, process_instance_id());
}
//------------------------------------------------------------------------------
auto root_logger::_log_compiler_info() -> void {
    info("built with ${complrName} compiler for ${archtcture} architecture")
      .tag(identifier{"Compiler"})
      .arg(
        identifier{"complrName"},
        identifier{"string"},
        compiler_name(),
        string_view{"not_available"})
      .arg(
        identifier{"archtcture"},
        identifier{"string"},
        architecture_name(),
        string_view{"not_available"})
      .arg_func([](logger_backend& backend) {
          if(const auto version_major{compiler_version_major()}) {
              backend.add_integer(
                identifier{"complrMajr"},
                identifier{"VrsnMajor"},
                extract(version_major));
          }
          if(const auto version_minor{compiler_version_minor()}) {
              backend.add_integer(
                identifier{"complrMinr"},
                identifier{"VrsnMinor"},
                extract(version_minor));
          }
          if(const auto version_patch{compiler_version_patch()}) {
              backend.add_integer(
                identifier{"complrPtch"},
                identifier{"VrsnPatch"},
                extract(version_patch));
          }
      });
}
//------------------------------------------------------------------------------
} // namespace eagine
