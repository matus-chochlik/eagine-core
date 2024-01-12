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

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.identifier;
import eagine.core.utility;
import eagine.core.runtime;
import eagine.core.logging;
import eagine.core.console;
import eagine.core.progress;
import :interface;
import :parent;

namespace eagine {
//------------------------------------------------------------------------------
export class application_config;
export class main_ctx;
export class main_ctx_object;
//------------------------------------------------------------------------------
/// @brief Buffer borrowed from the main context usable by a main_ctxt_object
/// @ingroup main_context
export class main_ctx_buffer : public memory::buffer {
public:
    main_ctx_buffer(main_ctx_object& parent) noexcept
      : _parent{parent} {}
    main_ctx_buffer(main_ctx_object& parent, memory::buffer buf) noexcept
      : memory::buffer{std::move(buf)}
      , _parent{parent} {}
    main_ctx_buffer(main_ctx_object& parent, span_size_t size) noexcept
      : _parent{parent} {
        get(size);
    }
    main_ctx_buffer(main_ctx_object& parent, span_size_t size, nothing_t) noexcept
      : _parent{parent} {
        get(size).clear();
    }

    main_ctx_buffer(main_ctx_buffer&&) = delete;
    main_ctx_buffer(const main_ctx_buffer&) = delete;
    auto operator=(main_ctx_buffer&&) = delete;
    auto operator=(const main_ctx_buffer&) = delete;

    auto get(span_size_t size) noexcept -> main_ctx_buffer&;

    ~main_ctx_buffer() noexcept;

    auto buffers() const noexcept -> memory::buffer_pool&;

private:
    main_ctx_object& _parent;
};
//------------------------------------------------------------------------------
export template <typename T>
auto application_config_initial(
  application_config& config,
  const string_view key,
  T& initial,
  const string_view tag) noexcept -> T&;

/// @brief Base class for main context objects.
/// @ingroup main_context
///
/// Objects that want to get access to the main context object and the services
/// it provides (like logging), should inherit from this class.
export class main_ctx_object : public named_logging_object {
    using base = named_logging_object;

public:
    /// @brief Initialization from object id and parent.
    main_ctx_object(const identifier obj_id, main_ctx_parent parent) noexcept
      : base{_make_base(obj_id, parent)} {}

    main_ctx_object(main_ctx_object&&) noexcept = default;
    main_ctx_object(const main_ctx_object&) noexcept = default;
    auto operator=(main_ctx_object&&) noexcept -> main_ctx_object& = default;
    auto operator=(const main_ctx_object&) noexcept
      -> main_ctx_object& = default;
    ~main_ctx_object() noexcept = default;

    /// @brief Returns this as main_ctx_object_parent_info.
    auto as_parent() const noexcept -> main_ctx_object_parent_info {
        return {*this};
    }

    /// @brief Returns a reference to the main context singleton.
    auto main_context() const noexcept -> main_ctx&;

    /// @brief Returns the process id.
    [[nodiscard]] auto process_instance_id() const noexcept
      -> process_instance_id_t;

    /// @brief Returns a reference to the application config object.
    [[nodiscard]] auto app_config() const noexcept -> application_config&;

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
        return application_config_initial(app_config(), key, value, tag)
          .value_or(initial);
    }

    /// @brief Returns a reference to the console object.
    /// @see cio_warning
    /// @see cio_error
    [[nodiscard]] auto cio() const noexcept -> const console&;

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
    [[nodiscard]] auto progress() const noexcept -> const activity_progress&;

    /// @brief Returns a reference to the shared scheduler object.
    [[nodiscard]] auto scheduler() const noexcept -> action_scheduler&;

    /// @brief Returns a reference to the shared workshop object.
    [[nodiscard]] auto workers() const noexcept -> workshop&;

    /// @brief Locates the specified service object.
    [[nodiscard]] auto locate_service(identifier type_id) const noexcept
      -> optional_reference<main_ctx_service>;

    /// @brief Locates the specified Service object.
    /// @see locate
    template <std::derived_from<main_ctx_service> Service>
    [[nodiscard]] auto locate() const noexcept -> optional_reference<Service> {
        return locate_service(Service::static_type_id())
          .as(std::type_identity<Service>{});
    }

    /// @brief Locates the specified service object.
    [[nodiscard]] auto share_service(identifier type_id) const noexcept
      -> shared_holder<main_ctx_service>;

    /// @brief Locates the specified Service object.
    /// @see locate
    template <std::derived_from<main_ctx_service> Service>
    [[nodiscard]] auto share() const noexcept -> shared_holder<Service> {
        return share_service(Service::static_type_id())
          .as(std::type_identity<Service>{});
    }

private:
    static auto _make_base(const identifier, main_ctx_parent) noexcept -> base;
};
//------------------------------------------------------------------------------
} // namespace eagine
