/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.main_ctx:interface;

import std;
import eagine.core.build_info;
import eagine.core.types;
import eagine.core.identifier;
import eagine.core.memory;
import eagine.core.utility;
import eagine.core.runtime;
import eagine.core.logging;
import eagine.core.progress;
import eagine.core.console;

namespace eagine {
//------------------------------------------------------------------------------
export class application_config;
export class process_watchdog;

export class user_info;
export class system_info;
export class main_ctx_object;
//------------------------------------------------------------------------------
/// @brief Interface for classes that can be located through the main context.
/// @ingroup main_context
/// @see main_ctx_getters
/// @see main_ctx_setters
export struct main_ctx_service : interface<main_ctx_service> {
    virtual auto type_id() const noexcept -> identifier = 0;
};

export template <typename Derived>
struct main_ctx_service_impl : main_ctx_service {
    auto type_id() const noexcept -> identifier final {
        return Derived::static_type_id();
    }
};
//------------------------------------------------------------------------------
/// @brief Interface for classes providing access to main context singletons.
/// @ingroup main_context
/// @see main_ctx_setters
export struct main_ctx_setters : interface<main_ctx_setters> {
    /// @brief Injects the service object to main context.
    virtual void inject(shared_holder<main_ctx_service>) = 0;

    /// @brief Registers a activity progress observer.
    virtual auto register_observer(progress_observer&) -> bool = 0;

    /// @brief Un-registers a activity progress observer.
    virtual void unregister_observer(progress_observer&) = 0;

    /// @brief Assigns a function to be called on progress update.
    virtual void set_progress_update_callback(
      const callable_ref<bool() noexcept>& callback,
      const std::chrono::milliseconds min_interval) = 0;

    /// @brief Resets the function called on progress update.
    virtual void reset_progress_update_callback() noexcept = 0;
};

/// @brief Interface for classes providing access to main context singletons.
/// @ingroup main_context
/// @see main_ctx
export struct main_ctx_getters : interface<main_ctx_getters> {
    /// @brief Returns the associated setter object (may return nullptr).
    virtual auto setters() noexcept -> optional_reference<main_ctx_setters> = 0;

    /// @brief Does potentially expensive initialization and caching.
    virtual auto preinitialize() noexcept -> main_ctx_getters& = 0;

    /// @brief Does one iteration of repeated update of internal components.
    virtual auto update() noexcept -> main_ctx_getters& = 0;

    /// @brief Returns this process instance id. Not equal to system PID.
    virtual auto instance_id() const noexcept -> process_instance_id_t = 0;

    /// @brief Returns the default allocator.
    virtual auto default_allocator() const noexcept
      -> const memory::shared_byte_allocator& = 0;

    /// @brief Returns the executable path.
    virtual auto exe_path() const noexcept -> string_view = 0;

    /// @brief Returns the application name.
    virtual auto app_name() const noexcept -> string_view = 0;

    /// @brief Returns a reference to program arguments wrapper.
    virtual auto args() const noexcept -> const program_args& = 0;

    /// @brief Returns a reference to compiler information instance.
    /// @see build
    virtual auto compiler() const noexcept -> const compiler_info& = 0;

    /// @brief Returns a reference to build information instance.
    /// @see compiler
    virtual auto build() const noexcept -> const build_info& = 0;

    /// @brief Returns a reference to build version information instance.
    /// @see compiler
    virtual auto version() const noexcept -> const version_info& = 0;

    /// @brief Returns a reference to system information object.
    /// @see user
    virtual auto system() noexcept -> system_info& = 0;

    /// @brief Returns a reference to user information object.
    /// @see system
    virtual auto user() noexcept -> user_info& = 0;

    /// @brief Returns a reference to the console object.
    virtual auto cio() noexcept -> const console& = 0;

    /// @brief Returns a reference to the root logger object.
    virtual auto log() noexcept -> const logger& = 0;

    /// @brief Returns a reference to the root activity object.
    virtual auto progress() noexcept -> const activity_progress& = 0;

    /// @brief Returns a reference to application config object.
    virtual auto config() noexcept -> application_config& = 0;

    /// @brief Returns a reference to process watchdog object.
    virtual auto watchdog() noexcept -> process_watchdog& = 0;

    /// @brief Returns a reference to shared data compressor object.
    virtual auto compressor() noexcept -> data_compressor& = 0;

    /// @brief Returns a reference to shared scheduler object.
    virtual auto scheduler() noexcept -> action_scheduler& = 0;

    /// @brief Returns a reference to shared workshop object.
    virtual auto workers() noexcept -> workshop& = 0;

    /// @brief Returns a reference to global buffer pool.
    virtual auto buffers() noexcept -> memory::buffer_pool& = 0;

    /// @brief Returns a reference to shared temporary buffer.
    virtual auto scratch_space() noexcept -> memory::buffer& = 0;

    /// @brief Locates a service object by it's type name.
    virtual auto locate_service(identifier type_id) noexcept
      -> optional_reference<main_ctx_service> = 0;

    /// @brief Locates a service object by it's type name.
    virtual auto share_service(identifier type_id) noexcept
      -> shared_holder<main_ctx_service> = 0;

    /// @brief Fills the given memory block with random bytes.
    virtual void fill_with_random_bytes(memory::block) noexcept = 0;

    /// @brief Fills the given span with uniform random values in range <0, 1>.
    virtual void random_uniform_01(memory::span<float>) noexcept = 0;

    /// @brief Fills the given span with uniform random values in range <-1, 1>.
    virtual void random_uniform_11(memory::span<float>) noexcept = 0;

    /// @brief Fills the given span with normal-distributed random values.
    virtual void random_normal(memory::span<float>) noexcept = 0;

    /// @brief Fills the given span with identifier characters
    virtual void random_identifier(memory::span<char>) noexcept = 0;

    /// @brief Fills the given span with identifier characters
    virtual void random_identifier(std::string&) noexcept = 0;
};

export auto try_get_main_ctx() noexcept -> optional_reference<main_ctx_getters>;
//------------------------------------------------------------------------------
/// @brief Registers the activity progress observer.
/// @ingroup main_context
/// @see main_ctx_setters
export auto register_progress_observer(
  main_ctx_getters& ctx,
  progress_observer& observer) noexcept -> bool {
    assert(ctx.setters());
    return ctx.setters()
      .and_then([&](auto& s) { return tribool{s.register_observer(observer)}; })
      .or_false();
}

/// @brief Un-registers the activity progress observer.
/// @ingroup main_context
/// @see main_ctx_setters
export void unregister_progress_observer(
  main_ctx_getters& ctx,
  progress_observer& observer) noexcept {
    assert(ctx.setters());
    return ctx.setters().and_then(
      [&](auto& s) { s.unregister_observer(observer); });
}

/// @brief Assigns a progress update callback function.
/// @ingroup main_context
/// @see main_ctx_setters
/// @see reset_progress_update_callback
export void set_progress_update_callback(
  main_ctx_getters& ctx,
  const callable_ref<bool() noexcept>& callback,
  const std::chrono::milliseconds min_interval) noexcept {
    assert(ctx.setters());
    return ctx.setters().and_then(
      [&](auto& s) { s.set_progress_update_callback(callback, min_interval); });
}

/// @brief Resets the progress update callback function.
/// @ingroup main_context
/// @see main_ctx_setters
/// @see set_progress_update_callback
export void reset_progress_update_callback(main_ctx_getters& ctx) noexcept {
    assert(ctx.setters());
    return ctx.setters().and_then(
      [](auto& s) { s.reset_progress_update_callback(); });
}
//------------------------------------------------------------------------------
/// @brief Structure storing customization options for main context.
/// @ingroup main_context
export struct main_ctx_options {

    /// @brief The application name. Defaults to the executable name.
    std::string app_name{};

    /// @brief The application root logger identifier.
    identifier app_id{"RootLogger"};

    /// @brief Options for the root logger.
    root_logger_options logger_opts{};

    /// @brief Options for the console.
    console_options console_opts{};
};
//------------------------------------------------------------------------------
} // namespace eagine
