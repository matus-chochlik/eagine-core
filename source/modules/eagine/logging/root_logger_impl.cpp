/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>
#if __has_include(<sys/types.h>) && __has_include(<unistd.h>)
#include <sys/types.h>
#include <unistd.h>
#ifndef EAGINE_POSIX
#define EAGINE_POSIX 1
#endif
#else
#ifndef EAGINE_POSIX
#define EAGINE_POSIX 0
#endif
#endif

module eagine.core.logging;

import std;
import eagine.core.types;
import eagine.core.debug;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.identifier;
import eagine.core.reflection;
import eagine.core.build_config;
import eagine.core.build_info;
import eagine.core.valid_if;
import eagine.core.utility;
import eagine.core.runtime;
import :config;
import :entry;
import :backend;
import <csignal>;

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
  const log_stream_info& info) -> unique_holder<logger_backend> {

    auto lock_type{log_backend_lock::mutex};
    if(args.has("--log-use-spinlock")) {
        lock_type = log_backend_lock::spinlock;
    } else if(args.has("--log-use-no-lock")) {
        lock_type = log_backend_lock::none;
    }

    auto format{log_data_format::json};
    if(args.has("--log-format-json")) {
        format = log_data_format::json;
    } else if(args.has("--log-format-xml")) {
        format = log_data_format::xml;
    }

    for(auto& arg : args) {
        if(arg.is_long_tag("use-null-log")) {
            return make_null_log_backend();
        } else if(arg.is_long_tag("use-cerr-log")) {
            return make_ostream_log_backend(std::cerr, info, format, lock_type);
        } else if(arg.is_long_tag("use-syslog")) {
            return make_syslog_log_backend(info, lock_type);
        } else if(arg.is_long_tag("use-asio-nw-log")) {
            string_view nw_addr;
            if(arg.next() and not arg.next().starts_with("-")) {
                nw_addr = arg.next();
            } else if(const auto env_var{get_environment_variable(
                        "EAGINE_LOG_NETWORK_ADDRESS")}) {
                nw_addr = *env_var;
            }
            return make_asio_tcpipv4_log_backend(
              nw_addr, info, format, lock_type);
        } else if(arg.is_long_tag("use-asio-log")) {
            string_view path;
            if(arg.next() and not arg.next().starts_with("-")) {
                path = arg.next();
            } else if(const auto env_var{
                        get_environment_variable("EAGINE_LOG_LOCAL_PATH")}) {
                path = *env_var;
            }
            return make_asio_local_log_backend(path, info, format, lock_type);
        }
    }
    switch(opts.default_backend) {
        case default_log_backend::cerr:
            return make_ostream_log_backend(std::cerr, info, format, lock_type);
        case default_log_backend::syslog:
            return make_syslog_log_backend(info, lock_type);
        case default_log_backend::local_socket:
            return make_asio_local_log_backend({}, info, format, lock_type);
        case default_log_backend::network_socket:
            return make_asio_tcpipv4_log_backend({}, info, format, lock_type);
        case default_log_backend::none:
            return make_null_log_backend();
        case default_log_backend::unspecified:
            break;
    }

    return make_proxy_log_backend(info);
}
//------------------------------------------------------------------------------
auto root_logger_init_backend(
  const program_args& args,
  root_logger_options& opts) -> shared_holder<logger_backend> {
    if(opts.forced_backend) {
        return std::move(opts.forced_backend);
    }

    log_stream_info info{};
    info.min_severity = default_log_severity();

    for(auto arg = args.first(); arg; arg = arg.next()) {
        if(arg.is_long_tag("min-log-severity")) {
            if(assign_if_fits(arg.next(), info.min_severity)) {
                arg = arg.next();
            }
        } else if(arg.is_long_tag("log-identity")) {
            if(assign_if_fits(arg.next(), info.log_identity)) {
                arg = arg.next();
            }
        } else if(arg.is_long_tag("session-identity")) {
            if(assign_if_fits(arg.next(), info.session_identity)) {
                arg = arg.next();
            }
        }
    }

    auto backend{root_logger_choose_backend(args, opts, info)};

    return backend;
}
//------------------------------------------------------------------------------
class root_logger_signals {
public:
    root_logger_signals() noexcept;
    ~root_logger_signals() noexcept;

    auto is_initialized() const noexcept -> bool;
    void initialize(root_logger*) noexcept;
    void reset() noexcept;

private:
    static auto _logger() noexcept -> optional_reference<root_logger>;
    static auto _logger_ptr() noexcept -> root_logger*&;

    static void _handle_usr1(int);
    static void _handle_usr2(int);

    using _sighandler_t = void(int);
    _sighandler_t* _usr1_handler{nullptr};
    _sighandler_t* _usr2_handler{nullptr};
};
//------------------------------------------------------------------------------
auto root_logger_signals::_logger_ptr() noexcept -> root_logger*& {
    static root_logger* ptr{nullptr};
    return ptr;
}
//------------------------------------------------------------------------------
root_logger_signals::root_logger_signals() noexcept
  : _usr1_handler{std::signal(SIGUSR1, &_handle_usr1)}
  , _usr2_handler{std::signal(SIGUSR2, &_handle_usr2)} {}
//------------------------------------------------------------------------------
root_logger_signals::~root_logger_signals() noexcept {
    [[maybe_unused]] const auto unused_1{std::signal(SIGUSR1, _usr1_handler)};
    [[maybe_unused]] const auto unused_2{std::signal(SIGUSR2, _usr2_handler)};
}
//------------------------------------------------------------------------------
auto root_logger_signals::is_initialized() const noexcept -> bool {
    return _logger_ptr() != nullptr;
}
//------------------------------------------------------------------------------
void root_logger_signals::initialize(root_logger* parent) noexcept {
    assert(not is_initialized());
    _logger_ptr() = parent;
}
//------------------------------------------------------------------------------
void root_logger_signals::reset() noexcept {
    _logger_ptr() = nullptr;
}
//------------------------------------------------------------------------------
auto root_logger_signals::_logger() noexcept
  -> optional_reference<root_logger> {
    return {_logger_ptr()};
}
//------------------------------------------------------------------------------
void root_logger_signals::_handle_usr1(int) {
    _logger().member(&root_logger::make_more_verbose);
}
//------------------------------------------------------------------------------
void root_logger_signals::_handle_usr2(int) {
    _logger().member(&root_logger::make_less_verbose);
}
//------------------------------------------------------------------------------
void cleanup() noexcept;
//------------------------------------------------------------------------------
// root logger
//------------------------------------------------------------------------------
void root_logger::_init(root_logger_signals& signals) noexcept {
    signals.initialize(this);
}
//------------------------------------------------------------------------------
void root_logger::_reset(root_logger_signals& signals) noexcept {
    signals.reset();
}
//------------------------------------------------------------------------------
auto root_logger::_log_args(const program_args& args) -> void {
    // pwd
    std::error_code ec;
    info("working directory: ${workDir}")
      .tag("pwd")
      .arg("workDir", std::filesystem::current_path(ec));
    // args
    auto args_entry{info("program arguments:")};
    args_entry.tag("ProgArgs");
    args_entry.arg("cmd", "FsPath", args.command().get());
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
auto root_logger::_log_build_info() -> void {
    info("source version information")
      .tag("BuildInfo")
      .arg("onValgrind", running_on_valgrind())
      .arg("lowProfile", tribool{low_profile_build})
      .arg("debug", tribool{debug_build});
}
//------------------------------------------------------------------------------
auto root_logger::_log_instance_info() -> void {
    std::array<char, 1024> hname{};
#if EAGINE_POSIX
    try {
        if(::gethostname(hname.data(), hname.size() - 1) != 0) {
            hname[0] = '\0';
        }
    } catch(...) {
    }
#endif
    info("instance information")
      .tag("Instance")
      .arg("hostname", "Hostname", std::string{hname.data()})
#if EAGINE_POSIX
      .arg("osPID", "PID", ::getpid())
#endif
      .arg("instanceId", process_instance_id());
}
//------------------------------------------------------------------------------
auto root_logger::_log_compiler_info() -> void {
    const string_view not_available{"N/A"};
    info("built with ${complrName} compiler for ${archtcture} architecture")
      .tag("Compiler")
      .arg("complrName", "str", compiler_name(), not_available)
      .arg("archtcture", "str", architecture_name(), not_available)
      .arg_func([](logger_backend& backend) {
          if(const auto version_major{compiler_version_major()}) {
              backend.add_integer("complrMajr", "VrsnMajor", *version_major);
          }
          if(const auto version_minor{compiler_version_minor()}) {
              backend.add_integer("complrMinr", "VrsnMinor", *version_minor);
          }
          if(const auto version_patch{compiler_version_patch()}) {
              backend.add_integer("complrPtch", "VrsnPatch", *version_patch);
          }
      });
}
//------------------------------------------------------------------------------
void root_logger::make_more_verbose() noexcept {
    backend().member(&logger_backend::make_more_verbose);
}
//------------------------------------------------------------------------------
void root_logger::make_less_verbose() noexcept {
    backend().member(&logger_backend::make_less_verbose);
}
//------------------------------------------------------------------------------
static root_logger_signals _root_logger_signals{};
//------------------------------------------------------------------------------
root_logger::root_logger(
  identifier logger_id,
  const program_args& args,
  root_logger_options& opts) noexcept
  : logger{logger_id, {root_logger_init_backend(args, opts)}} {
    _init(_root_logger_signals);
    begin_log();

    for(auto arg = args.first(); arg; arg = arg.next()) {
        if(arg.is_long_tag("--log-active-state")) {
            if(
              identifier::can_be_encoded(arg.next().get()) and
              identifier::can_be_encoded(arg.next().next().get())) {
                active_state(
                  identifier{arg.next().get()},
                  identifier{arg.next().next().get()});
            }
        }
    }

    _log_args(args);
    _log_instance_info();
    _log_build_info();
    _log_git_info();
    _log_os_info();
    _log_compiler_info();
}
//------------------------------------------------------------------------------
root_logger::~root_logger() noexcept {
    finish_log();
    _reset(_root_logger_signals);
}
//------------------------------------------------------------------------------
} // namespace eagine
