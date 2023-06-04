/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.main_ctx;

import std;
import eagine.core.build_config;
import eagine.core.build_info;
import eagine.core.debug;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.identifier;
import eagine.core.runtime;
import eagine.core.logging;
import eagine.core.utility;
import eagine.core.valid_if;
import eagine.core.value_tree;

namespace eagine {
//------------------------------------------------------------------------------
class application_config_impl {
public:
    application_config_impl(logger& parent, main_ctx_getters& ctx)
      : _main_ctx{ctx}
      , _log{"AppCfgImpl", parent} {
        // front is empty and is filled out later
        _tag_list.emplace_back();

        if(auto hostname{ctx.system().hostname()}) {
            _hostname = std::move(*hostname);
            _tag_list.emplace_back(_hostname);
        }

        if(const auto architecture{architecture_tag()}) {
            _tag_list.push_back(*architecture);
        }

        std::array<string_view, 2> tag_labels{
          {{"--instance"}, {"--config-tag"}}};

        for(const auto label : tag_labels) {
            if(const auto arg{ctx.args().find(label)}) {
                if(const auto tag_arg{arg.next()}) {
                    _tag_list.push_back(tag_arg.get());
                }
            }
        }

        if constexpr(debug_build) {
            _tag_list.emplace_back("debug");
            if(running_on_valgrind()) {
                _tag_list.emplace_back("slowexec");
            }
        }
        _config_name.reserve(128);
    }

    auto find_compound_attribute(
      const string_view key,
      const string_view tag) noexcept -> valtree::compound_attribute {
        try {
            const std::lock_guard<decltype(_mutex)> lck{_mutex};
            _tag_list.front() = tag;
            const auto tags{skip_until(
              view(_tag_list), [](auto t) { return not t.is_empty(); })};

            auto app_name{_main_ctx.app_name()};
            if(auto found{_find_config_of(app_name, key, tags)}) {
                return found;
            }
            app_name = strip_prefix(app_name, string_view{"eagine-"});
            if(auto found{_find_config_of(app_name, key, tags)}) {
                return found;
            }
            if(auto arg{_main_ctx.args().find("--config-group")}) {
                if(const auto group_arg{arg.next()}) {
                    if(auto found{
                         _find_config_of(group_arg.get(), key, tags)}) {
                        return found;
                    }
                }
            }
            if(auto found{_find_config_of("defaults", key, tags)}) {
                return found;
            }
            if(auto found{_find_in_secrets_fs(key, tags)}) {
                return found;
            }
        } catch(...) {
            _log.error("exception while loading configuration value '${key}'")
              .arg("key", key);
        }
        return {};
    }

    void link(
      [[maybe_unused]] const string_view key,
      [[maybe_unused]] const string_view tag,
      application_config_value_loader& loader) noexcept {
        _loaders.insert(&loader);
    }

    void unlink(
      [[maybe_unused]] const string_view key,
      [[maybe_unused]] const string_view tag,
      application_config_value_loader& loader) noexcept {
        _loaders.erase(&loader);
    }

    auto reload() noexcept -> bool {
        bool result{true};
        const std::lock_guard<decltype(_mutex)> lck{_mutex};
        if(not _loaders.empty()) {
            _open_configs.clear();
            for(auto loader : _loaders) {
                result = loader->reload() and result;
            }
        }
        return result;
    }

private:
    auto _cat(const string_view l, const string_view r) noexcept
      -> const std::string& {
        return append_to(r, assign_to(l, _config_name));
    }

    auto _cat(
      const string_view a,
      const string_view b,
      const string_view c,
      const string_view d) noexcept -> const std::string& {
        return append_to(
          d, append_to(c, append_to(b, assign_to(a, _config_name))));
    }

    auto _secrets_fs_path() const noexcept -> const std::filesystem::path& {
        static const std::filesystem::path p{"/run/secrets"};
        return p;
    }

    auto _find_in_secrets_fs(
      const string_view key,
      const span<const string_view> tags) noexcept
      -> valtree::compound_attribute {
        if(auto comp{_get_config(_secrets_fs_path())}) {
            if(auto attr{comp.find(basic_string_path(key), tags)}) {
                return {std::move(comp), std::move(attr)};
            }
        }
        return {};
    }

    auto _find_config_of(
      const string_view group,
      const string_view key,
      const span<const string_view> tags) noexcept
      -> valtree::compound_attribute {
        for(auto path_kind :
            {config_path_kind::user,
             config_path_kind::system,
             config_path_kind::install}) {
            for(const auto tag : tags) {
                if(auto found{_find_in(
                     _config_path(_cat(group, "@", tag, ".yaml"), path_kind),
                     key,
                     tags)}) {
                    return found;
                }
                if(auto found{_find_in(
                     _config_path(_cat(group, "@", tag, ".json"), path_kind),
                     key,
                     tags)}) {
                    return found;
                }
            }
            if(auto found{_find_in(
                 _config_path(_cat(group, ".yaml"), path_kind), key, tags)}) {
                return found;
            }
            if(auto found{_find_in(
                 _config_path(_cat(group, ".json"), path_kind), key, tags)}) {
                return found;
            }
        }
        return {};
    }

    auto _find_in(
      const std::filesystem::path& cfg_path,
      const string_view key,
      const span<const string_view> tags) -> valtree::compound_attribute {
        if(not cfg_path.empty()) {
            if(is_regular_file(cfg_path) or is_fifo(cfg_path)) {
                if(auto comp{_get_config(cfg_path)}) {
                    if(auto attr{comp.find(
                         basic_string_path(key, split_by, "."), tags)}) {
                        return {comp, attr};
                    }
                }
            }
        }
        return {};
    }

    enum class config_path_kind { user, system, install };

    auto _config_path(const string_view name, const config_path_kind kind)
      -> std::filesystem::path {
        std::filesystem::path config_path;
        if(kind == config_path_kind::user) {
            if(auto config_dir{_main_ctx.user().config_dir_path()}) {
                config_path.append(std::string_view{*config_dir});
                config_path.append("eagine");
                config_path.append(std::string_view{name});
            }
        } else if(kind == config_path_kind::system) {
            if(auto config_dir{_main_ctx.system().config_dir_path()}) {
                config_path.append(std::string_view{*config_dir});
                config_path.append("eagine");
                config_path.append(std::string_view{name});
            }
        } else if(kind == config_path_kind::install) {
            if(auto config_dir{_main_ctx.build().config_dir_path()}) {
                config_path.append(std::string_view{*config_dir});
                config_path.append("eagine");
                config_path.append(std::string_view{name});
            }
        }
        return config_path;
    }

    auto _open_config(const std::filesystem::path& cfg_path)
      -> valtree::compound {
        if(is_directory(cfg_path)) {
            return valtree::from_filesystem_path(cfg_path.string(), _log);
        } else if(cfg_path.extension() == ".yaml") {
            if(file_contents content{cfg_path.string()}) {
                return valtree::from_yaml_text(memory::as_chars(content), _log);
            }
        } else if(cfg_path.extension() == ".json") {
            if(file_contents content{cfg_path.string()}) {
                return valtree::from_json_text(memory::as_chars(content), _log);
            }
        }
        return {};
    }

    auto _get_config(const std::filesystem::path& cfg_path)
      -> valtree::compound {
        if(exists(cfg_path)) {
            const auto path_str{canonical(cfg_path).string()};
            auto pos = _open_configs.find(path_str);
            if(pos == _open_configs.end()) {
                pos =
                  _open_configs.emplace(path_str, _open_config(cfg_path)).first;
            }
            return pos->second;
        }
        return {};
    }

    main_ctx_getters& _main_ctx;
    logger _log;
    std::recursive_mutex _mutex;
    std::map<std::string, valtree::compound> _open_configs;
    std::set<application_config_value_loader*> _loaders;
    std::vector<string_view> _tag_list;
    std::string _config_name;
    std::string _hostname;
};
//------------------------------------------------------------------------------
// application_config
//------------------------------------------------------------------------------
void application_config::_log_could_not_read_value(
  const string_view key,
  const string_view tag,
  const optionally_valid<string_view> val,
  const config_source src) noexcept {
    _log.error("could not read configuration ${key} value '${val}' from ${src}")
      .arg("key", key)
      .arg("tag", tag)
      .arg("val", val.or_default())
      .arg("src", src);
}
//------------------------------------------------------------------------------
auto application_config::_find_comp_attr(
  const string_view key,
  const string_view tag) noexcept -> valtree::compound_attribute {
    return _impl(_log, _main_ctx)->find_compound_attribute(key, tag);
}
//------------------------------------------------------------------------------
auto application_config::_prog_args() noexcept -> const program_args& {
    return _main_ctx.args();
}
//------------------------------------------------------------------------------
application_config::application_config(main_ctx_getters& ctx) noexcept
  : basic_config{ctx.args()}
  , _main_ctx{ctx}
  , _log{"AppConfig", ctx.log()} {}
//------------------------------------------------------------------------------
auto application_config::is_set(
  const string_view key,
  const string_view tag) noexcept -> bool {
    if(basic_config::is_set(key, tag)) {
        return true;
    }
    if(const auto attr{_find_comp_attr(key, tag)}) {
        bool flag{false};
        if(attr.select_value(flag, from_config)) {
            return flag;
        }
    }
    return false;
}
//------------------------------------------------------------------------------
auto application_config::fetch_string(
  const string_view key,
  const string_view tag,
  std::string& dest) noexcept -> bool {
    if(basic_config::fetch_string(key, tag, dest)) {
        return true;
    }
    if(const auto attr{_find_comp_attr(key, tag)}) {
        if(attr.select_value(dest, from_config)) {
            return true;
        }
    }
    return false;
}
//------------------------------------------------------------------------------
void application_config::link(
  const string_view key,
  const string_view tag,
  application_config_value_loader& loader) noexcept {
    _impl(_log, _main_ctx)->link(key, tag, loader);
}
//------------------------------------------------------------------------------
void application_config::unlink(
  const string_view key,
  const string_view tag,
  application_config_value_loader& loader) noexcept {
    _impl(_log, _main_ctx)->unlink(key, tag, loader);
}
//------------------------------------------------------------------------------
auto application_config::reload() noexcept -> bool {
    return _impl(_log, _main_ctx)
      .member(&application_config_impl::reload)
      .value_or(false);
}
//------------------------------------------------------------------------------
} // namespace eagine
