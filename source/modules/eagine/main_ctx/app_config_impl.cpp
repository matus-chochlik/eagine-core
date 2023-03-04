/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.main_ctx;

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
import std;

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
            _hostname = std::move(extract(hostname));
            _tag_list.emplace_back(_hostname);
        }

        if(const auto architecture{architecture_tag()}) {
            _tag_list.push_back(extract(architecture));
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
                result = extract(loader).reload() and result;
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
                config_path.append(std::string_view{extract(config_dir)});
                config_path.append("eagine");
                config_path.append(std::string_view{name});
            }
        } else if(kind == config_path_kind::system) {
            if(auto config_dir{_main_ctx.system().config_dir_path()}) {
                config_path.append(std::string_view{extract(config_dir)});
                config_path.append("eagine");
                config_path.append(std::string_view{name});
            }
        } else if(kind == config_path_kind::install) {
            if(auto config_dir{_main_ctx.build().config_dir_path()}) {
                config_path.append(std::string_view{extract(config_dir)});
                config_path.append("eagine");
                config_path.append(std::string_view{name});
            }
        }
        return config_path;
    }

    auto _open_config(const std::filesystem::path& cfg_path)
      -> valtree::compound {
        if(cfg_path.extension() == ".yaml") {
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
      -> valtree::compound& {
        const auto path_str{canonical(cfg_path).string()};
        auto pos = _open_configs.find(path_str);
        if(pos == _open_configs.end()) {
            pos = _open_configs.emplace(path_str, _open_config(cfg_path)).first;
        }
        return pos->second;
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
auto application_config::_find_comp_attr(
  const string_view key,
  const string_view tag) noexcept -> valtree::compound_attribute {
    if(const auto impl{_impl()}) {
        return extract(impl).find_compound_attribute(key, tag);
    }
    return {};
}
//------------------------------------------------------------------------------
auto application_config::_prog_args() noexcept -> const program_args& {
    return _main_ctx.args();
}
//------------------------------------------------------------------------------
auto application_config::_prog_arg_name(string_view key) noexcept
  -> std::string {
    std::string arg_name;
    arg_name.reserve(integer(safe_add(3, key.size())));
    arg_name.append("--");
    for(auto c : key) {
        if(c == '.' or c == '_') {
            c = '-';
        }
        arg_name.append(&c, 1U);
    }
    return arg_name;
}
//------------------------------------------------------------------------------
auto application_config::_find_prog_arg(const string_view key) noexcept
  -> program_arg {
    return _prog_args().find(_prog_arg_name(key));
}
//------------------------------------------------------------------------------
auto application_config::_eval_env_var(const string_view key) noexcept
  -> std::optional<string_view> {
    std::string arg_name;
    arg_name.reserve(integer(safe_add(7, key.size())));
    arg_name.append("EAGINE_");
    for(auto c : key) {
        if(c == '.') {
            c = '_';
        } else {
            c = static_cast<char>(std::toupper(c));
        }
        arg_name.append(&c, 1U);
    }
    return get_environment_variable(arg_name);
}
//------------------------------------------------------------------------------
application_config::application_config(main_ctx_getters& ctx) noexcept
  : _main_ctx{ctx}
  , _log{"AppConfig", ctx.log()} {}
//------------------------------------------------------------------------------
auto application_config::is_set(
  const string_view key,
  const string_view tag) noexcept -> bool {
    if(const auto attr{_find_comp_attr(key, tag)}) {
        bool flag{false};
        if(attr.select_value(flag, from_config)) {
            return flag;
        }
    }
    if(const auto arg{_find_prog_arg(key)}) {
        bool result{true};
        arg.parse_next(result, from_config, _log.debug_stream());
        return result;
    }
    if(const auto opt_val{_eval_env_var(key)}) {
        if(const auto converted{from_string<bool>(extract(opt_val))}) {
            return extract(converted);
        }
    }
    return false;
}
//------------------------------------------------------------------------------
auto application_config::_impl() noexcept -> application_config_impl* {
    if(not _pimpl) [[unlikely]] {
        try {
            _pimpl = std::make_shared<application_config_impl>(_log, _main_ctx);
        } catch(...) {
        }
    }
    return _pimpl.get();
}
//------------------------------------------------------------------------------
void application_config::link(
  const string_view key,
  const string_view tag,
  application_config_value_loader& loader) noexcept {
    if(const auto impl{_impl()}) {
        return extract(impl).link(key, tag, loader);
    }
}
//------------------------------------------------------------------------------
void application_config::unlink(
  const string_view key,
  const string_view tag,
  application_config_value_loader& loader) noexcept {
    if(const auto impl{_impl()}) {
        return extract(impl).unlink(key, tag, loader);
    }
}
//------------------------------------------------------------------------------
auto application_config::reload() noexcept -> bool {
    if(const auto impl{_impl()}) {
        return extract(impl).reload();
    }
    return false;
}
//------------------------------------------------------------------------------
} // namespace eagine
