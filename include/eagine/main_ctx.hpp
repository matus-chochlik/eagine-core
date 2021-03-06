/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#ifndef EAGINE_MAIN_CTX_HPP
#define EAGINE_MAIN_CTX_HPP

#include "console/console_opts.hpp"
#include "identifier.hpp"
#include "logging/root_logger_opts.hpp"
#include "main_ctx_fwd.hpp"
#include "program_args.hpp"

namespace eagine {

/// @brief Structure storing customization options for main context.
/// @ingroup main_context
struct main_ctx_options {

    /// @brief The application name. Defaults to the executable name.
    std::string app_name{};

    /// @brief The application root logger identifier.
    identifier app_id{"RootLogger"};

    /// @brief Options for the root logger.
    root_logger_options logger_opts{};

    /// @brief Options for the console.
    console_options console_opts{};
};

extern auto main(main_ctx& ctx) -> int;

auto main_impl(int, const char**, main_ctx_options&, int (*)(main_ctx&)) -> int;

inline auto main_impl(int argc, const char** argv, main_ctx_options& options)
  -> int {
    return main_impl(argc, argv, options, &main);
}

inline auto default_main(
  int argc,
  const char** argv,
  int (*main_func)(main_ctx&)) -> int {
    main_ctx_options options{};
    return main_impl(argc, argv, options, main_func);
}

inline auto default_main(int argc, const char** argv) -> int {
    return default_main(argc, argv, &main);
}

/// @brief Class for a singleton object providing useful information and services.
/// @ingroup main_context
///
/// A single instance of this class is initialized in the main function
/// and constructs several useful utility service objects that can be shared
/// throughout the system.
class main_ctx : public main_ctx_getters {
public:
    main_ctx(main_ctx_getters&) noexcept;

    /// @brief Not move constructible.
    main_ctx(main_ctx&&) = delete;

    /// @brief Not copy constructible.
    main_ctx(const main_ctx&) = delete;

    /// @brief Not move assignable.
    auto operator=(main_ctx&&) = delete;

    /// @brief Not copy asignable.
    auto operator=(const main_ctx&) = delete;

    ~main_ctx() noexcept override;

    static auto try_get() noexcept -> main_ctx* {
        return _single_ptr();
    }

    /// @brief Returns the single instance.
    static auto get() noexcept -> main_ctx& {
        EAGINE_ASSERT(_single_ptr());
        return *_single_ptr();
    }

    auto setters() noexcept -> main_ctx_setters* final {
        return _source.setters();
    }

    auto preinitialize() noexcept -> main_ctx& final;

    auto instance_id() const noexcept -> process_instance_id_t final {
        return _instance_id;
    }

    auto default_allocator() const noexcept
      -> const memory::shared_byte_allocator& final {
        return _default_alloc;
    }

    auto exe_path() const noexcept -> string_view final {
        return _exe_path;
    }

    auto app_name() const noexcept -> string_view final {
        return _app_name;
    }

    auto args() const noexcept -> const program_args& final {
        return _args;
    }

    auto compiler() const noexcept -> const compiler_info& final {
        return _cmplr_info;
    }

    auto build() const noexcept -> const build_info& final {
        return _bld_info;
    }

    auto system() noexcept -> system_info& final {
        return _sys_info;
    }

    auto user() noexcept -> user_info& final {
        return _usr_info;
    }

    auto cio() noexcept -> const console& final {
        return _cio;
    }

    auto log() noexcept -> const logger& final {
        return _log;
    }

    auto progress() noexcept -> const activity_progress& final {
        return _progress;
    }

    auto config() noexcept -> application_config& final {
        return _app_config;
    }

    auto watchdog() noexcept -> process_watchdog& final {
        return _watchdog;
    }

    auto bus() noexcept -> message_bus& final {
        return _source.bus();
    }

    auto compressor() noexcept -> data_compressor& final {
        return _compressor;
    }

    auto scratch_space() noexcept -> memory::buffer& final {
        return _scratch_space;
    }

private:
    const process_instance_id_t _instance_id;
    main_ctx_getters& _source;
    const memory::shared_byte_allocator& _default_alloc;
    const program_args& _args;
    const compiler_info& _cmplr_info;
    const build_info& _bld_info;
    const console& _cio;
    const logger& _log;
    const activity_progress& _progress;
    process_watchdog& _watchdog;
    application_config& _app_config;
    system_info& _sys_info;
    user_info& _usr_info;
    memory::buffer& _scratch_space;
    data_compressor& _compressor;
    string_view _exe_path;
    string_view _app_name;

    static auto _single_ptr() noexcept -> main_ctx*&;
};

} // namespace eagine

#if !EAGINE_CORE_LIBRARY || defined(EAGINE_IMPLEMENTING_CORE_LIBRARY)
#include <eagine/main_ctx.inl>
#endif

#endif // EAGINE_MAIN_CTX_HPP
