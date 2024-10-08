/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.main_ctx;

import std;
import eagine.core.build_info;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.identifier;
import eagine.core.utility;
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
    main_ctx(main_ctx_getters&);

    /// @brief Not move constructible.
    main_ctx(main_ctx&&) = delete;

    /// @brief Not copy constructible.
    main_ctx(const main_ctx&) = delete;

    /// @brief Not move assignable.
    auto operator=(main_ctx&&) = delete;

    /// @brief Not copy asignable.
    auto operator=(const main_ctx&) = delete;

    ~main_ctx() noexcept override;

    [[nodiscard]] static auto try_get() noexcept
      -> optional_reference<main_ctx> {
        return _single_ptr();
    }

    /// @brief Returns the single instance.
    [[nodiscard]] static auto get() noexcept -> main_ctx& {
        assert(_single_ptr());
        return *_single_ptr();
    }

    [[nodiscard]] auto setters() noexcept
      -> optional_reference<main_ctx_setters> final {
        return _source.setters();
    }

    auto preinitialize() noexcept -> main_ctx& final;
    auto update() noexcept -> main_ctx& final;

    [[nodiscard]] auto instance_id() const noexcept
      -> process_instance_id_t final {
        return _instance_id;
    }

    [[nodiscard]] auto default_allocator() const noexcept
      -> const memory::shared_byte_allocator& final {
        return _default_alloc;
    }

    [[nodiscard]] auto exe_path() const noexcept -> string_view final {
        return _exe_path;
    }

    [[nodiscard]] auto app_name() const noexcept -> string_view final {
        return _app_name;
    }

    [[nodiscard]] auto args() const noexcept -> const program_args& final {
        return _args;
    }

    [[nodiscard]] auto compiler() const noexcept -> const compiler_info& final {
        return _cmplr_info;
    }

    [[nodiscard]] auto build() const noexcept -> const build_info& final {
        return _bld_info;
    }

    [[nodiscard]] auto version() const noexcept -> const version_info& final {
        return _ver_info;
    }

    [[nodiscard]] auto system() noexcept -> system_info& final {
        return _sys_info;
    }

    [[nodiscard]] auto default_chunk_size() noexcept -> span_size_t {
        return system().memory_page_size().value_or(4 * 1024);
    }

    [[nodiscard]] auto user() noexcept -> user_info& final {
        return _usr_info;
    }

    [[nodiscard]] auto cio() noexcept -> const console& final {
        return _cio;
    }

    [[nodiscard]] auto log() noexcept -> const logger& final {
        return _log;
    }

    [[nodiscard]] auto progress() noexcept -> const activity_progress& final {
        return _progress;
    }

    [[nodiscard]] auto config() noexcept -> application_config& final {
        return _app_config;
    }

    [[nodiscard]] auto watchdog() noexcept -> process_watchdog& final {
        return _watchdog;
    }

    [[nodiscard]] auto compressor() noexcept -> data_compressor& final {
        return _compressor;
    }

    [[nodiscard]] auto scheduler() noexcept -> action_scheduler& final {
        return _scheduler;
    };

    [[nodiscard]] auto workers() noexcept -> workshop& final {
        return _workers;
    };

    [[nodiscard]] auto buffers() noexcept -> memory::buffer_pool& final {
        return _buffers;
    }

    [[nodiscard]] auto scratch_space() noexcept -> memory::buffer& final {
        return _scratch_space;
    }

    [[nodiscard]] auto locate_service(identifier type_id) noexcept
      -> optional_reference<main_ctx_service> final {
        return _source.locate_service(type_id);
    }

    template <std::derived_from<main_ctx_service> Service>
    [[nodiscard]] auto locate() noexcept -> optional_reference<Service> {
        return locate_service(Service::static_type_id())
          .as(std::type_identity<Service>{});
    }

    [[nodiscard]] auto share_service(identifier type_id) noexcept
      -> shared_holder<main_ctx_service> final {
        return _source.share_service(type_id);
    }

    template <std::derived_from<main_ctx_service> Service>
    [[nodiscard]] auto share() noexcept -> shared_holder<Service> {
        return share_service(Service::static_type_id())
          .as(std::type_identity<Service>{});
    }

    void fill_with_random_bytes(memory::block dest) noexcept final {
        _source.fill_with_random_bytes(dest);
    }

    void random_uniform_01(memory::span<float> dest) noexcept final {
        _source.random_uniform_01(dest);
    }

    auto random_uniform_01() noexcept -> float {
        float result{0.F};
        _source.random_uniform_01(cover_one(result));
        return result;
    }

    void random_uniform_11(memory::span<float> dest) noexcept final {
        _source.random_uniform_11(dest);
    }

    auto random_uniform_11() noexcept -> float {
        float result{0.F};
        _source.random_uniform_11(cover_one(result));
        return result;
    }

    void random_normal(memory::span<float> dest) noexcept final {
        _source.random_normal(dest);
    }

    auto random_normal() noexcept -> float {
        float result{0.F};
        _source.random_normal(cover_one(result));
        return result;
    }

    auto random_identifier() noexcept -> identifier final {
        return _source.random_identifier();
    }

    void random_identifier(memory::span<char> dest) noexcept final {
        _source.random_identifier(dest);
    }

    void random_identifier(std::string& dest) noexcept final {
        _source.random_identifier(dest);
    }

    auto random_identifier(std::size_t length) noexcept -> std::string {
        std::string result;
        result.resize(length);
        _source.random_identifier(result);
        return result;
    }

    [[nodiscard]] auto encrypt_shared(
      memory::const_block nonce,
      memory::const_block input,
      memory::buffer& output) noexcept -> bool;

    [[nodiscard]] auto decrypt_shared(
      memory::const_block nonce,
      memory::const_block input,
      memory::buffer& output) noexcept -> bool;

    [[nodiscard]] auto encrypt_shared(
      memory::const_block nonce,
      string_view input,
      memory::buffer& output) noexcept -> bool;

    [[nodiscard]] auto decrypt_shared(
      memory::const_block nonce,
      memory::const_block input,
      std::string& output) noexcept -> bool;

    [[nodiscard]] auto encrypt_shared_password(
      memory::const_block nonce,
      const string_view key,
      const string_view tag,
      memory::buffer& encrypted) noexcept -> bool;

    [[nodiscard]] auto encrypt_shared_password(
      memory::const_block nonce,
      const string_view key,
      memory::buffer& encrypted) noexcept -> bool {
        return encrypt_shared_password(nonce, key, {}, encrypted);
    }

    [[nodiscard]] auto matches_encrypted_shared_password(
      memory::const_block nonce,
      const string_view key,
      const string_view tag,
      memory::const_block encrypted) noexcept -> bool;

    [[nodiscard]] auto matches_encrypted_shared_password(
      memory::const_block nonce,
      const string_view key,
      memory::const_block encrypted) noexcept -> bool {
        return matches_encrypted_shared_password(nonce, key, {}, encrypted);
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
    memory::buffer_pool& _buffers;
    memory::buffer& _scratch_space;
    data_compressor& _compressor;
    action_scheduler& _scheduler;
    workshop& _workers;
    string_view _exe_path;
    string_view _app_name;

    static auto _single_ptr() noexcept -> main_ctx*&;
};
//------------------------------------------------------------------------------
} // namespace eagine

