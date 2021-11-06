/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/compiler_info.hpp>
#include <eagine/environment.hpp>
#include <eagine/git_info.hpp>
#include <eagine/logging/asio_backend.hpp>
#include <eagine/logging/null_backend.hpp>
#include <eagine/logging/ostream_backend.hpp>
#include <eagine/logging/proxy_backend.hpp>
#include <eagine/logging/syslog_backend.hpp>
#include <eagine/logging/type/program_args.hpp>
#include <eagine/process.hpp>
#include <cerrno>
#include <iostream>

namespace eagine {
//------------------------------------------------------------------------------
static constexpr auto default_log_severity() noexcept {
    return static_cast<log_event_severity>(
      static_cast<std::underlying_type_t<log_event_severity>>(
        log_event_severity::EAGINE_MIN_LOG_SEVERITY) +
      1);
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto root_logger_choose_backend(
  const program_args& args,
  const root_logger_options& opts,
  const log_stream_info& info) -> std::unique_ptr<logger_backend> {
    std::unique_ptr<logger_backend> result{};

    for(auto& arg : args) {
        if(arg.is_long_tag("use-null-log")) {
            return std::make_unique<null_log_backend>();
        } else if(arg.is_long_tag("use-cerr-log")) {
            return std::make_unique<ostream_log_backend<>>(std::cerr, info);
        } else if(arg.is_long_tag("use-cout-log")) {
            return std::make_unique<ostream_log_backend<>>(std::cout, info);
        } else if(arg.is_long_tag("use-syslog")) {
            return std::make_unique<syslog_log_backend<>>(info);
        } else if(arg.is_long_tag("use-asio-nw-log")) {
            string_view nw_addr;
            if(arg.next() && !arg.next().starts_with("-")) {
                nw_addr = arg.next();
            } else if(auto env_var{get_environment_variable(
                        "EAGINE_LOG_NETWORK_ADDRESS")}) {
                nw_addr = extract(env_var);
            }
            return std::make_unique<asio_tcpipv4_ostream_log_backend<>>(
              nw_addr, info);
#if EAGINE_HAS_ASIO_LOCAL_LOG_BACKEND
        } else if(arg.is_long_tag("use-asio-log")) {
            return std::make_unique<asio_local_ostream_log_backend<>>(info);
#endif
        }
    }

    if(opts.default_no_log) {
        return std::make_unique<null_log_backend>();
    }

    return std::make_unique<proxy_log_backend>(info);
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
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
EAGINE_LIB_FUNC
auto root_logger::_log_args(const program_args& args) -> void {
    auto args_entry{info("program arguments:")};
    args_entry.tag(EAGINE_ID(ProgArgs));
    args_entry.arg(EAGINE_ID(cmd), args.command());
    for(const auto& arg : args) {
        args_entry.arg(EAGINE_ID(arg), arg);
    }
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto root_logger::_log_git_info() -> void {
    const string_view n_a{"N/A"};
    info("source version information")
      .tag(EAGINE_ID(GitInfo))
      .arg(EAGINE_ID(gitBranch), EAGINE_ID(GitBranch), config_git_branch(), n_a)
      .arg(EAGINE_ID(gitHashId), EAGINE_ID(GitHash), config_git_hash_id(), n_a)
      .arg(EAGINE_ID(gitDate), EAGINE_ID(RFC2822), config_git_date(), n_a)
      .arg(EAGINE_ID(gitDescrib), EAGINE_ID(str), config_git_describe(), n_a)
      .arg(EAGINE_ID(gitVersion), EAGINE_ID(str), config_git_version(), n_a);
}
EAGINE_LIB_FUNC
//------------------------------------------------------------------------------
auto root_logger::_log_instance_info() -> void {
    info("instance information")
      .tag(EAGINE_ID(Instance))
      .arg(EAGINE_ID(instanceId), make_process_instance_id());
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto root_logger::_log_compiler_info() -> void {
    info("built with ${complrName} compiler for ${Archtcture} architecture")
      .tag(EAGINE_ID(Compiler))
      .arg(
        EAGINE_ID(complrName),
        EAGINE_ID(string),
        compiler_name(),
        string_view{"unknown"})
      .arg(
        EAGINE_ID(Archtcture),
        EAGINE_ID(string),
        architecture_name(),
        string_view{"unknown"})
      .arg_func([](logger_backend& backend) {
          if(auto version_major{compiler_version_major()}) {
              backend.add_integer(
                EAGINE_ID(complrMajr),
                EAGINE_ID(VrsnMajor),
                extract(version_major));
          }
          if(auto version_minor{compiler_version_minor()}) {
              backend.add_integer(
                EAGINE_ID(complrMinr),
                EAGINE_ID(VrsnMinor),
                extract(version_minor));
          }
          if(auto version_patch{compiler_version_patch()}) {
              backend.add_integer(
                EAGINE_ID(complrPtch),
                EAGINE_ID(VrsnPatch),
                extract(version_patch));
          }
      });
}
//------------------------------------------------------------------------------
} // namespace eagine
