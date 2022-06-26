/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.main_ctx;

import eagine.core.build_info;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.runtime;
import eagine.core.logging;
import eagine.core.progress;
import eagine.core.console;
export import :interface;
export import :parent;
export import :object;
export import :app_config;
export import :system_info;
export import :user_info;
export import :watchdog;
export import :app_config;
export import :storage;
export import :main;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Class for a singleton object providing useful information and services.
/// @ingroup main_context
///
/// A single instance of this class is initialized in the main function
/// and constructs several useful utility service objects that can be shared
/// throughout the system.
export class main_ctx : public main_ctx_getters {
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
        assert(_single_ptr());
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

    auto version() const noexcept -> const version_info& final {
        return _ver_info;
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
    const version_info& _ver_info;
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
//------------------------------------------------------------------------------
} // namespace eagine

