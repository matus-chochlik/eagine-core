/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.logging:root_logger;

import eagine.core.types;
import eagine.core.identifier;
import eagine.core.runtime;
import :backend;
import :logger;
import <memory>;

namespace eagine {
export class application_config;
//------------------------------------------------------------------------------
/// @brief Structure holding root logger customization options.
/// @ingroup logging
export struct root_logger_options {
    /// @brief The explicitly-specified backend to be used by the root logger.
    std::unique_ptr<logger_backend> forced_backend{};

    /// @brief Option saying that by default no logging should be done.
    bool default_no_log{false};
};
//------------------------------------------------------------------------------
/// @brief Class typically used as the per-application single root of logger hierarchy.
/// @ingroup logging
export class root_logger : public logger {
public:
    /// @brief Constructor initializing from and id, program arguments and options.
    root_logger(
      identifier logger_id,
      const program_args& args,
      root_logger_options& opts)
      : logger{logger_id, {_init_backend(args, opts)}} {
        _log_args(args);
        _log_instance_info();
        _log_git_info();
        _log_os_info();
        _log_compiler_info();
    }

    /// @brief Constructor initializing from program arguments and logger options.
    root_logger(const program_args& args, root_logger_options& opts)
      : root_logger{identifier{"RootLogger"}, args, opts} {}

private:
    static auto _init_backend(const program_args&, root_logger_options&)
      -> std::unique_ptr<logger_backend>;

    auto _log_os_info() -> void;
    auto _log_git_info() -> void;
    auto _log_compiler_info() -> void;
    auto _log_instance_info() -> void;
    auto _log_args(const program_args&) -> void;
};
//------------------------------------------------------------------------------
} // namespace eagine
