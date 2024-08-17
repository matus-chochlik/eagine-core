/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.main_ctx:app_config;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.identifier;
import eagine.core.reflection;
import eagine.core.container;
import eagine.core.valid_if;
import eagine.core.runtime;
import eagine.core.logging;
import eagine.core.value_tree;
import :interface;

namespace eagine {
//------------------------------------------------------------------------------
class application_config_impl;

/// @brief Internal interface for reloadable configuration values.
/// @ingroup main_context
/// @see application_config
/// @see application_reconfig_value
/// @note Do not use directly.
export struct application_config_value_loader
  : interface<application_config_value_loader> {

    /// @brief Reloads the value from the application config.
    virtual auto reload() noexcept -> bool = 0;
};

/// @brief Enumeration of potential sources of configuration values.
/// @ingroup main_context
export enum class config_source {
    /// @brief Command-line arguments.
    arguments,
    /// @brief Environment variable.
    environment,
    /// @brief Configuration file.
    filesystem,
    /// @brief Key-value store.
    key_value_store
};

export template <>
struct enumerator_traits<config_source> {
    static constexpr auto mapping() noexcept {
        return enumerator_map_type<config_source, 4>{
          {{"arguments", config_source::arguments},
           {"environment", config_source::environment},
           {"filesystem", config_source::filesystem},
           {"key_value_store", config_source::key_value_store}}};
    }
};

/// @brief Class for reading application configuration.
/// @ingroup main_context
///
/// This class allow to read application configuration values from from
/// environment variables, command line arguments and/or configuration files.
export class application_config final : public basic_config {

public:
    application_config(main_ctx_getters& ctx) noexcept;

    using basic_config::is_set;

    /// @brief Checks is the boolean option identified by @p key is set to true.
    auto is_set(const string_view key, const string_view tag) noexcept
      -> bool final;

    using basic_config::fetch_string;

    auto fetch_string(
      const string_view key,
      const string_view tag,
      std::string& dest) noexcept -> bool final;

    /// @brief Fetches the configuration value identified by @p key, into @p dest.
    template <typename T>
    auto fetch(
      const string_view key,
      T& dest,
      const string_view tag = {}) noexcept -> bool {
        if(const auto arg{find_program_arg(key, tag)}) {
            if(assign_if_fits(arg.next().get(), dest)) {
                return true;
            } else {
                _log_could_not_read_value(
                  key, tag, arg.get(), config_source::arguments);
            }
        }
        if(const auto opt_val{eval_environment_var(key, tag)}) {
            if(assign_if_fits(*opt_val, dest)) {
                return true;
            } else {
                _log_could_not_read_value(
                  key, tag, opt_val, config_source::environment);
            }
        }
        if(const auto attr{_find_comp_attr(key, tag)}) {
            if(attr.fetch_value(dest)) {
                return true;
            } else {
                _log_could_not_read_value(
                  key, tag, attr.preview(), config_source::filesystem);
            }
        }
        return false;
    }

    template <typename Vector>
    [[nodiscard]] auto fetch_vector(
      const string_view key,
      Vector& dest,
      const string_view tag) noexcept -> bool {
        using T = typename Vector::value_type;
        const auto arg_name{program_arg_name(key, tag)};
        for(const auto arg : _prog_args()) {
            if(arg.is_tag(arg_name)) {
                T temp{};
                if(assign_if_fits(arg.next().get(), temp)) {
                    dest.emplace_back(std::move(temp));
                } else {
                    _log_could_not_read_value(
                      key, tag, arg.get(), config_source::arguments);
                    return false;
                }
            }
        }
        if(const auto opt_val{eval_environment_var(key, tag)}) {
            T temp{};
            if(assign_if_fits(*opt_val, temp)) {
                dest.emplace_back(std::move(temp));
            } else {
                _log_could_not_read_value(
                  key, tag, *opt_val, config_source::environment);
                return false;
            }
        }
        if(const auto attr{_find_comp_attr(key, tag)}) {
            const auto count(attr.value_count());
            if(count > 0) {
                dest.resize(safe_add(dest.size(), std_size(count)));
                if(not attr.fetch_values(tail(memory::cover(dest), count))) {
                    _log_could_not_read_value(
                      key, tag, attr.preview(), config_source::filesystem);
                    return false;
                }
            }
        }
        return true;
    }

    /// @brief Fetches the configuration values identified by @p key, into @p dest.
    template <typename T, typename A>
    auto fetch(
      const string_view key,
      std::vector<T, A>& dest,
      const string_view tag = {}) noexcept -> bool {
        return fetch_vector(key, dest, tag);
    }

    /// @brief Fetches the configuration values identified by @p key, into @p dest.
    template <typename T, std::size_t N, typename A>
    auto fetch(
      const string_view key,
      small_vector<T, N, A>& dest,
      const string_view tag = {}) noexcept -> bool {
        return fetch_vector(key, dest, tag);
    }

    /// @brief Fetches the configuration value identified by @p key, into @p dest.
    template <typename T, typename P>
    auto fetch(
      const string_view key,
      valid_if<T, P>& dest,
      const string_view tag = {}) noexcept -> bool {
        T temp{};
        if(fetch(key, temp, tag)) {
            if(dest.has_value(temp)) {
                dest = std::move(temp);
                return true;
            } else {
                _log.error("value '${value}' is not valid for '${key}'")
                  .arg("value", temp)
                  .arg("key", key);
            }
        }
        return false;
    }

    auto fetch(
      const string_view key,
      url& locator,
      const string_view tag = {}) noexcept -> bool {
        std::string temp{};
        if(fetch(key, temp, tag)) {
            locator = url{std::move(temp)};
            if(locator) {
                return true;
            }
            _log.error("value '${value}' is not valid for '${key}'")
              .arg("value", temp)
              .arg("key", key);
        }
        return false;
    }

    /// @brief Returns the configuration value or type @p T, identified by @p key.
    template <typename T>
    [[nodiscard]] auto get(
      const string_view key,
      const std::type_identity<T> = {}) noexcept -> optionally_valid<T> {
        T temp{};
        const auto fetched{fetch(key, temp)};
        return {std::move(temp), fetched};
    }

    /// @brief Fetches the configuration value identified by @p key, into @p init.
    template <typename T>
    auto init(
      const string_view key,
      T& initial,
      const string_view tag = {}) noexcept -> T {
        fetch(key, initial, tag);
        return initial;
    }

    /// @brief Links a value loader to this configuration instance.
    /// @see unlink
    /// @see reload
    void link(
      const string_view key,
      const string_view tag,
      application_config_value_loader& loader) noexcept;

    /// @brief Unlinks a value loader from this configuration instance.
    /// @see link
    /// @see reload
    void unlink(
      const string_view key,
      const string_view tag,
      application_config_value_loader& loader) noexcept;

    /// @brief Reloads the linked loadable configuration values.
    /// @see link
    /// @see unlink
    auto reload() noexcept -> bool;

private:
    main_ctx_getters& _main_ctx;
    logger _log;
    shared_holder<application_config_impl> _impl;

    void _log_could_not_read_value(
      const string_view key,
      const string_view tag,
      const optionally_valid<string_view> val,
      const config_source) noexcept;

    auto _find_comp_attr(const string_view key, const string_view tag) noexcept
      -> valtree::compound_attribute;

    auto _prog_args() noexcept -> const program_args&;
};
//------------------------------------------------------------------------------
export template <typename T>
auto application_config_initial(
  application_config& config,
  const string_view key,
  T& initial,
  const string_view tag) noexcept -> T& {
    config.init(key, initial, tag);
    return initial;
}
//------------------------------------------------------------------------------
// application config value
//------------------------------------------------------------------------------
/// @brief Class wrapping values that can be loaded from application_config.
/// @ingroup main_context
export template <
  typename T,
  typename As = std::add_const_t<T>&,
  bool Reloadable = false>
class application_config_value;

/// @brief Class wrapping values that can be reloaded from application_config.
/// @ingroup main_context
export template <typename T>
using application_reconfig_value =
  application_config_value<T, std::add_const_t<T>&, true>;
export class main_ctx_object;
//------------------------------------------------------------------------------
/// @brief Class wrapping values that can be loaded from application_config.
/// @ingroup main_context
export template <typename T, typename As>
class application_config_value<T, As, false> {
public:
    /// @brief Initialization from key, tag and optional initial value.
    application_config_value(
      application_config& config,
      const string_view key,
      const string_view tag,
      T initial = {}) noexcept
      : _value{application_config_initial(config, key, initial, tag)} {}

    /// @brief Initialization from key and optional initial value.
    application_config_value(
      application_config& config,
      const string_view key,
      T initial = {}) noexcept
      : application_config_value{config, key, {}, std::move(initial)} {}

    application_config_value(
      std::derived_from<main_ctx_object> auto& o,
      const string_view key,
      const string_view tag,
      T init = {}) noexcept
      : application_config_value{o.app_config(), key, tag, std::move(init)} {}

    application_config_value(
      std::derived_from<main_ctx_object> auto& o,
      const string_view key,
      T init = {}) noexcept
      : application_config_value{o.app_config(), key, std::move(init)} {}

    auto operator=(T new_value) -> auto& {
        _value = std::move(new_value);
        return *this;
    }

    [[nodiscard]] auto has_value() const noexcept -> bool {
        return true;
    }

    /// @brief Returns the stored value converted to the @p As type.
    [[nodiscard]] auto value() const noexcept -> As {
        return As(_value);
    }

    [[nodiscard]] auto value_or(const auto&) const noexcept -> As {
        return value();
    }

    /// @brief Return a const reference to the stored value.
    /// @see value
    [[nodiscard]] auto operator*() const noexcept -> const T& {
        return _value;
    }

    /// @brief Return a const pointer to the stored value.
    /// @see value
    [[nodiscard]] auto operator->() const noexcept -> const T* {
        return &_value;
    }

    /// @brief Implicit conversion of the stored value to the @p As type.
    /// @see value
    [[nodiscard]] operator As() const noexcept {
        return value();
    }

    template <typename U>
    [[nodiscard]] auto operator==(U&& that) const noexcept {
        return _value == that;
    }

    template <typename U>
    [[nodiscard]] auto operator!=(U&& that) const noexcept {
        return _value != that;
    }

    template <typename U>
    [[nodiscard]] auto operator<(U&& that) const noexcept {
        return _value < that;
    }

    template <typename U>
    [[nodiscard]] auto operator>(U&& that) const noexcept {
        return _value > that;
    }

    template <typename U>
    [[nodiscard]] auto operator<=(U&& that) const noexcept {
        return _value <= that;
    }

    template <typename U>
    [[nodiscard]] auto operator>=(U&& that) const noexcept {
        return _value >= that;
    }

protected:
    auto ref() noexcept -> T& {
        return _value;
    }

private:
    T _value{};
};
//------------------------------------------------------------------------------
class application_config_value_reloader
  : public application_config_value_loader {
protected:
    application_config_value_reloader(
      application_config& config,
      std::string key,
      std::string tag) noexcept
      : _config{config}
      , _key{std::move(key)}
      , _tag{std::move(tag)} {}

    auto config() const noexcept -> application_config& {
        return _config;
    }

    auto tag() const noexcept -> string_view {
        return _tag;
    }

    auto key() const noexcept -> string_view {
        return _key;
    }

private:
    application_config& _config;
    const std::string _key;
    const std::string _tag;
};
//------------------------------------------------------------------------------
/// @brief Class wrapping values that can be reloaded from application_config.
/// @ingroup main_context
export template <typename T, typename As>
class application_config_value<T, As, true> final
  : public application_config_value<T, As, false>
  , private application_config_value_reloader {
public:
    /// @brief Initialization from key, tag and optional initial value.
    application_config_value(
      application_config& config,
      std::string key,
      std::string tag,
      T initial = {}) noexcept
      : application_config_value<
          T,
          As,
          false>{config, key, tag, std::move(initial)}
      , application_config_value_reloader{
          config,
          std::move(key),
          std::move(tag)} {
        this->config().link(this->key(), this->tag(), *this);
    }

    /// @brief Initialization from key and optional initial value.
    application_config_value(
      application_config& config,
      std::string key,
      T initial = {}) noexcept
      : application_config_value{config, std::move(key), {}, initial} {}

    application_config_value(
      std::derived_from<main_ctx_object> auto& o,
      const string_view key,
      const string_view tag,
      T init = {}) noexcept
      : application_config_value{o.app_config(), key, tag, std::move(init)} {}

    application_config_value(
      std::derived_from<main_ctx_object> auto& o,
      const string_view key,
      T init = {}) noexcept
      : application_config_value{o.app_config(), key, std::move(init)} {}

    application_config_value(application_config_value&&) = delete;
    application_config_value(const application_config_value&) = delete;
    auto operator=(application_config_value&&) = delete;
    auto operator=(const application_config_value&) = delete;

    ~application_config_value() noexcept final {
        this->config().unlink(this->key(), this->tag(), *this);
    }

private:
    auto reload() noexcept -> bool final {
        return this->config().fetch(this->key(), this->ref(), this->tag());
    }
};
//------------------------------------------------------------------------------
} // namespace eagine

