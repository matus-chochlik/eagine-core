/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#ifndef EAGINE_MAIN_CTX_OBJECT_HPP
#define EAGINE_MAIN_CTX_OBJECT_HPP

#include "assert.hpp"
#include "identifier_t.hpp"
#include "logging/logger.hpp"
#include "main_ctx_fwd.hpp"
#include "overwrite_guard.hpp"

namespace eagine {

template <typename T>
auto application_config_initial(
  application_config& config,
  string_view key,
  T& initial,
  string_view tag) -> T&;

/// @brief Helper class used in the implementation of logging in main context
/// object.
/// @ingroup type_utils
/// @see main_ctx_object
/// @note Do not use directly.
class main_ctx_log_backend_getter {
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
class main_ctx_object
  : public overwrite_guard<>
  , public named_logging_object<main_ctx_log_backend_getter> {
    using base = named_logging_object<main_ctx_log_backend_getter>;

public:
    /// @brief Initialization from object id and parent.
    main_ctx_object(identifier obj_id, main_ctx_parent parent) noexcept
      : base{_make_base(obj_id, parent)} {}

    /// @brief Returns this as main_ctx_object_parent_info.
    auto as_parent() noexcept -> main_ctx_object_parent_info {
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
    auto cfg_init(string_view key, T initial, string_view tag = {}) -> T {
        return application_config_initial(app_config(), key, initial, tag);
    }

    /// @brief Reads and returns the configuration value identified by @p key.
    template <typename Extractable, typename T>
    auto cfg_extr(
      string_view key,
      T initial,
      string_view tag = {},
      type_identity<Extractable> = {}) -> T {
        Extractable value(initial);
        return extract_or(
          application_config_initial(app_config(), key, value, tag), initial);
    }

    /// @brief Returns a reference to the root activity object.
    auto progress() const noexcept -> const activity_progress&;

    /// @brief Returns a reference to the message bus object.
    auto bus() const noexcept -> message_bus&;

private:
    static auto _make_base(identifier obj_id, main_ctx_parent parent) noexcept
      -> base {
        if(parent._object) {
            return base{obj_id, static_cast<const base&>(*parent._object)};
        }
        if(parent._context) {
            return base{obj_id, main_ctx_log_backend_getter{*parent._context}};
        }
        return base{obj_id, main_ctx_log_backend_getter{}};
    }
};

} // namespace eagine

#endif // EAGINE_MAIN_CTX_OBJECT_HPP
