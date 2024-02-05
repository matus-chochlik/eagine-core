/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.logging:root_logger;

import std;
import eagine.core.types;
import eagine.core.identifier;
import eagine.core.runtime;
import :backend;
import :logger;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Enumeration of the default logger backend if not other is configured.
/// @ingroup logging
export enum class default_log_backend : std::uint8_t {
    /// @brief Unspecified
    unspecified,
    /// @brief Standard error
    cerr,
    /// @brief SysLog
    syslog,
    /// @brief Local socket
    local_socket,
    /// @brief Network  socket
    network_socket,
    /// @brief No logging
    none
};
//------------------------------------------------------------------------------
/// @brief Structure holding root logger customization options.
/// @ingroup logging
export struct root_logger_options {
    /// @brief The explicitly-specified backend to be used by the root logger.
    shared_holder<logger_backend> forced_backend{};

    /// @brief Option saying that by default no logging should be done.
    default_log_backend default_backend{default_log_backend::unspecified};
};
//------------------------------------------------------------------------------
struct root_logger_signals;
/// @brief Class typically used as the per-application single root of logger hierarchy.
/// @ingroup logging
export class root_logger : public logger {
public:
    /// @brief Constructor initializing from and id, program arguments and options.
    root_logger(
      identifier logger_id,
      const program_args& args,
      root_logger_options& opts) noexcept;

    /// @brief Constructor initializing from program arguments and logger options.
    root_logger(const program_args& args, root_logger_options& opts) noexcept
      : root_logger{"RootLogger", args, opts} {}

    root_logger(root_logger&&) = delete;
    root_logger(const root_logger&) = delete;
    auto operator=(root_logger&&) = delete;
    auto operator=(const root_logger&) = delete;
    ~root_logger() noexcept;

    void make_more_verbose() noexcept;
    void make_less_verbose() noexcept;

private:
    void _init(root_logger_signals&) noexcept;
    void _reset(root_logger_signals&) noexcept;

    auto _log_os_info() -> void;
    auto _log_git_info() -> void;
    auto _log_build_info() -> void;
    auto _log_compiler_info() -> void;
    auto _log_instance_info() -> void;
    auto _log_args(const program_args&) -> void;
};
//------------------------------------------------------------------------------
} // namespace eagine
