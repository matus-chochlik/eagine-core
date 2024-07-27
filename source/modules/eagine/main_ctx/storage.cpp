/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.main_ctx:storage;

import std;
import eagine.core.build_info;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.utility;
import eagine.core.valid_if;
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

namespace eagine {
//------------------------------------------------------------------------------
class main_ctx_storage
  : public main_ctx_getters
  , public main_ctx_setters {
public:
    main_ctx_storage(
      int argc,
      const char** argv,
      main_ctx_options& options) noexcept;

    auto setters() noexcept -> optional_reference<main_ctx_setters> final {
        return this;
    }

    auto preinitialize() noexcept -> main_ctx_storage& final {
        _sys_info.preinitialize();
        return *this;
    }

    auto update() noexcept -> main_ctx_storage& final {
        _scheduler.update();
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

    auto scheduler() noexcept -> action_scheduler& final {
        return _scheduler;
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

    void inject(shared_holder<main_ctx_service> service) final {
        if(service) {
            _services[service->type_id()] = service;
        }
    }

    auto locate_service(identifier type_id) noexcept
      -> optional_reference<main_ctx_service> final {
        return find(_services, type_id).or_default();
    }

    auto share_service(identifier type_id) noexcept
      -> shared_holder<main_ctx_service> final {
        return find(_services, type_id).or_default();
    }

    void fill_with_random_bytes(memory::block dest) noexcept final {
        eagine::fill_with_random_bytes(dest, _rand_eng);
    }

    void random_uniform_01(memory::span<float> dest) noexcept final {
        generate(dest, [this] { return _dist_uniform_float_01(_rand_eng); });
    }

    void random_uniform_11(memory::span<float> dest) noexcept final {
        generate(dest, [this] { return _dist_uniform_float_11(_rand_eng); });
    }

    void random_normal(memory::span<float> dest) noexcept final {
        generate(dest, [this] { return _dist_normal_float(_rand_eng); });
    }

    auto random_identifier() noexcept -> identifier final {
        return eagine::random_identifier(_rand_eng);
    }

    void random_identifier(memory::span<char> dest) noexcept final {
        eagine::fill_with_random_identifier(dest, _rand_eng);
    }

    void random_identifier(std::string& dest) noexcept final {
        eagine::fill_with_random_identifier(dest, _rand_eng);
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
    compiler_info _cmplr_info;
    version_info _ver_info{config_git_version_tuple()};
    console _console;
    root_logger _log_root;
    root_activity _progress_root{_log_root};
    system_info _sys_info{*this};
    user_info _usr_info{*this};
    process_watchdog _watchdog{*this};
    application_config _app_config{*this};
    memory::buffer_pool _buffers;
    memory::buffer _scratch_space{_default_alloc};
    data_compressor _compressor{_buffers};
    action_scheduler _scheduler{};
    workshop _workers{};
    std::string _exe_path{};
    std::string _app_name{};

    flat_map<identifier, shared_holder<main_ctx_service>> _services;

    valid_if_positive<std::default_random_engine::result_type> _rand_seed;
    std::default_random_engine::result_type _rand_init;
    std::default_random_engine _rand_eng;
    std::uniform_real_distribution<float> _dist_uniform_float_01{0.F, 1.F};
    std::uniform_real_distribution<float> _dist_uniform_float_11{-1.F, 1.F};
    std::normal_distribution<float> _dist_normal_float{0.F, 1.F};
};
//------------------------------------------------------------------------------
main_ctx_storage::main_ctx_storage(
  int argc,
  const char** argv,
  main_ctx_options& options) noexcept
  : _args{argc, argv}
  , _console{options.app_id, _args, options.console_opts}
  , _log_root{options.app_id, _args, options.logger_opts}
  , _app_name{options.app_name}
  , _rand_seed{_app_config
                 .get<std::default_random_engine::result_type>(
                   "application.random.seed")
                 .value_or(0U)}
  , _rand_init{_rand_seed.value_or(std::random_device{}())}
  , _rand_eng{_rand_init} {
    const auto fs_path = std::filesystem::path(to_string(_args.command()));
    if(_app_name.empty()) {
        _app_name = fs_path.stem().string();
    }
    _exe_path = fs_path.lexically_normal().string();

    _log_root.info("application ${appName} starting")
      .tag("appStart")
      .arg("appName", "AppName", _app_name)
      .arg("exePath", "FsPath", _exe_path);

    _log_root.info("using ${init} to initialize random generator")
      .tag("rndGenSeed")
      .arg("init", _rand_init);
}
//------------------------------------------------------------------------------
} // namespace eagine
