/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.main_ctx:storage;

import eagine.core.build_info;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.utility;
import eagine.core.identifier;
import eagine.core.container;
import eagine.core.runtime;
import eagine.core.logging;
import eagine.core.console;
import eagine.core.progress;
import :interface;
import :parent;
import :app_config;
import :system_info;
import :user_info;
import :watchdog;
import <chrono>;
import <filesystem>;
import <queue>;
import <string>;

namespace eagine {
//------------------------------------------------------------------------------
class main_ctx_storage
  : public main_ctx_getters
  , public main_ctx_setters {
public:
    main_ctx_storage(
      int argc,
      const char** argv,
      main_ctx_options& options) noexcept
      : _args{argc, argv}
      , _bld_info{}
      , _sys_info{}
      , _usr_info{}
      , _console{options.app_id, _args, options.console_opts}
      , _log_root{options.app_id, _args, options.logger_opts}
      , _progress_root{_log_root}
      , _watchdog{*this}
      , _app_config{*this}
      , _app_name{options.app_name} {
        const auto fs_path = std::filesystem::path(to_string(_args.command()));
        if(_app_name.empty()) {
            _app_name = fs_path.stem().string();
        }
        _exe_path = fs_path.lexically_normal().string();

        _log_root.info("application ${appName} starting")
          .arg("appName", _app_name)
          .arg("exePath", _exe_path);
    }

    auto setters() noexcept -> main_ctx_setters* final {
        return this;
    }

    auto preinitialize() noexcept -> main_ctx_storage& final {
        _sys_info.preinitialize();
        return *this;
    }

    auto instance_id() const noexcept -> process_instance_id_t final {
        return _instance_id;
    }

    auto default_allocator() const noexcept
      -> const memory::shared_byte_allocator& final {
        return _default_alloc;
    }

    auto args() const noexcept -> const program_args& final {
        return _args;
    }

    auto cio() noexcept -> const console& final {
        return _console;
    }

    auto log() noexcept -> const logger& final {
        return _log_root;
    }

    auto progress() noexcept -> const activity_progress& final {
        return _progress_root;
    }

    auto register_observer(progress_observer& observer) -> bool final {
        return _progress_root.register_observer(observer);
    }

    void unregister_observer(progress_observer& observer) final {
        _progress_root.unregister_observer(observer);
    }

    void set_progress_update_callback(
      const callable_ref<bool() noexcept>& callback,
      const std::chrono::milliseconds min_interval) final {
        _progress_root.set_update_callback(callback, min_interval);
    }

    void reset_progress_update_callback() noexcept final {
        _progress_root.reset_update_callback();
    }

    auto watchdog() noexcept -> process_watchdog& final {
        return _watchdog;
    }

    auto config() noexcept -> application_config& final {
        return _app_config;
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

    auto compressor() noexcept -> data_compressor& final {
        return _compressor;
    }

    auto workers() noexcept -> workshop& final {
        return _workers;
    }

    auto buffers() noexcept -> memory::buffer_pool& final {
        return _buffers;
    }

    auto scratch_space() noexcept -> memory::buffer& final {
        return _scratch_space;
    }

    void inject(std::shared_ptr<main_ctx_service> service) final {
        if(service) {
            _services[service->type_id()] = service;
        }
    }

    auto locate_service(identifier type_id) noexcept
      -> std::shared_ptr<main_ctx_service> final {
        if(const auto pos{_services.find(type_id)}; pos != _services.end()) {
            return std::get<1>(*pos);
        }
        return {};
    }

    auto exe_path() const noexcept -> string_view final {
        return {_exe_path};
    }

    auto app_name() const noexcept -> string_view final {
        return {_app_name};
    }

private:
    const process_instance_id_t _instance_id{process_instance_id()};
    memory::shared_byte_allocator _default_alloc{
      memory::default_byte_allocator()};
    program_args _args;
    build_info _bld_info;
    version_info _ver_info;
    system_info _sys_info;
    user_info _usr_info;
    console _console;
    root_logger _log_root;
    root_activity _progress_root;
    compiler_info _cmplr_info;
    process_watchdog _watchdog;
    application_config _app_config;
    memory::buffer_pool _buffers;
    memory::buffer _scratch_space{_default_alloc};
    data_compressor _compressor{_buffers};
    workshop _workers{};
    std::string _exe_path{};
    std::string _app_name{};

    flat_map<identifier, std::shared_ptr<main_ctx_service>> _services;
};
//------------------------------------------------------------------------------
} // namespace eagine
