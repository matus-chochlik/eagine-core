/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.main_ctx:object;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.identifier;
import eagine.core.runtime;
import eagine.core.logging;
import eagine.core.console;
import eagine.core.progress;
import :interface;
import :parent;
import <concepts>;

namespace eagine {
//------------------------------------------------------------------------------
export class application_config;
export class main_ctx;
export class main_ctx_object;
//------------------------------------------------------------------------------
export template <typename T>
auto application_config_initial(
  application_config& config,
  const string_view key,
  T& initial,
  const string_view tag) noexcept -> T&;

/// @brief Helper class used in the implementation of logging in main context
/// object.
/// @ingroup type_utils
/// @see main_ctx_object
/// @note Do not use directly.
export class main_ctx_log_backend_getter {
public:
    main_ctx_log_backend_getter() noexcept;
    main_ctx_log_backend_getter(main_ctx_getters&) noexcept;

    auto operator()() const noexcept -> auto* {
        return _backend;
    }

private:
    logger_backend* const _backend{nullptr};
};

/// @brief Base class for main context objects.
/// @ingroup main_context
///
/// Objects that want to get access to the main context object and the services
/// it provides (like logging), should inherit from this class.
export class main_ctx_object
  : public named_logging_object<main_ctx_log_backend_getter> {
    using base = named_logging_object<main_ctx_log_backend_getter>;

public:
    /// @brief Initialization from object id and parent.
    main_ctx_object(const identifier obj_id, main_ctx_parent parent) noexcept
      : base{_make_base(obj_id, parent)} {}

    /// @brief Returns this as main_ctx_object_parent_info.
    auto as_parent() const noexcept -> main_ctx_object_parent_info {
        return {*this};
    }

    /// @brief Returns a reference to the main context singleton.
    auto main_context() const noexcept -> main_ctx&;

    /// @brief Returns the process id.
    auto process_instance_id() const noexcept -> process_instance_id_t;

    /// @brief Returns a reference to the application config object.
    auto app_config() const noexcept -> application_config&;

    /// @brief Reads and returns the configuration value identified by @p key.
    template <typename T>
    auto cfg_init(
      const string_view key,
      T initial,
      const string_view tag = {}) noexcept -> T {
        return application_config_initial(app_config(), key, initial, tag);
    }

    /// @brief Reads and returns the configuration value identified by @p key.
    template <typename Extractable, typename T>
    auto cfg_extr(
      const string_view key,
      T initial,
      const string_view tag = {},
      const std::type_identity<Extractable> = {}) noexcept -> T {
        Extractable value(initial);
        return extract_or(
          application_config_initial(app_config(), key, value, tag), initial);
    }

    /// @brief Returns a reference to the console object.
    /// @see cio_warning
    /// @see cio_error
    auto cio() const noexcept -> const console&;

    /// @brief Shorthand for cio().print(...)
    /// @see cio
    auto cio_print(const console_entry_kind kind, const string_view format)
      const noexcept -> console_entry;

    /// @brief Shorthand for cio().print(...)
    /// @see cio
    /// @see cio_warning
    /// @see cio_error
    auto cio_print(const string_view format) const noexcept -> console_entry;

    /// @brief Shorthand for cio().warning(...)
    /// @see cio
    /// @see cio_print
    /// @see cio_error
    auto cio_warning(const string_view format) const noexcept -> console_entry;

    /// @brief Shorthand for cio().error(...)
    /// @see cio
    /// @see cio_print
    /// @see cio_warning
    auto cio_error(const string_view format) const noexcept -> console_entry;

    /// @brief Returns a reference to the root activity object.
    auto progress() const noexcept -> const activity_progress&;

    /// @brief Locates the specified service object.
    auto locate_service(identifier type_id) const noexcept
      -> std::shared_ptr<main_ctx_service>;

    /// @brief Locates the specified Service object.
    /// @see locate
    template <std::derived_from<main_ctx_service> Service>
    auto locate() const noexcept -> std::shared_ptr<Service> {
        return std::dynamic_pointer_cast<Service>(
          locate_service(Service::static_type_id()));
    }

private:
    static auto _make_base(
      const identifier obj_id,
      main_ctx_parent parent) noexcept -> base {
        if(parent.has_object()) {
            return base{obj_id, static_cast<const base&>(parent.object())};
        }
        if(parent.has_context()) {
            return base{obj_id, main_ctx_log_backend_getter{parent.context()}};
        }
        return base{obj_id, main_ctx_log_backend_getter{}};
    }
};
//------------------------------------------------------------------------------
} // namespace eagine
