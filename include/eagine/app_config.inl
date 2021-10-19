/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/config/basic.hpp>
#include <eagine/compiler_info.hpp>
#include <eagine/environment.hpp>
#include <eagine/file_contents.hpp>
#include <eagine/interop/valgrind.hpp>
#include <eagine/main_ctx.hpp>
#include <eagine/memory/span_algo.hpp>
#include <eagine/user_info.hpp>
#include <eagine/valid_if/not_empty.hpp>
#include <eagine/value_tree/json.hpp>
#include <eagine/value_tree/yaml.hpp>
#include <cctype>
#include <filesystem>
#include <map>
#include <mutex>
#include <set>

namespace eagine {
//------------------------------------------------------------------------------
class application_config_impl : public main_ctx_object {
public:
    application_config_impl(main_ctx_parent parent)
      : main_ctx_object{EAGINE_ID(AppCfgImpl), parent} {
        // front is empty and is filled out later
        _tag_list.emplace_back();

        if(auto hostname{main_context().system().hostname()}) {
            _hostname = std::move(extract(hostname));
            _tag_list.emplace_back(_hostname);
        }

        if(const auto architecture{architecture_tag()}) {
            _tag_list.push_back(extract(architecture));
        }

        std::array<string_view, 2> tag_labels{
          {{"--instance"}, {"--config-tag"}}};

        for(const auto label : tag_labels) {
            if(const auto arg{main_context().args().find(label)}) {
                if(auto tag_arg{arg.next()}) {
                    _tag_list.push_back(tag_arg.get());
                }
            }
        }

        if constexpr(EAGINE_DEBUG) {
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
              view(_tag_list), [](auto t) { return !t.is_empty(); })};

            auto app_name{main_context().app_name()};
            if(auto found{_find_config_of(app_name, key, tags)}) {
                return found;
            }
            app_name = strip_prefix(app_name, string_view{"eagine-"});
            if(auto found{_find_config_of(app_name, key, tags)}) {
                return found;
            }
            if(auto arg{main_context().args().find("--config-group")}) {
                if(auto group_arg{arg.next()}) {
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
            log_error("exception while loading configuration value '${key}'")
              .arg(EAGINE_ID(key), key);
        }
        return {};
    }

    void link(
      const string_view key,
      const string_view tag,
      application_config_value_loader& loader) noexcept {
        EAGINE_MAYBE_UNUSED(key);
        EAGINE_MAYBE_UNUSED(tag);
        _loaders.insert(&loader);
    }

    void unlink(
      const string_view key,
      const string_view tag,
      application_config_value_loader& loader) noexcept {
        EAGINE_MAYBE_UNUSED(key);
        EAGINE_MAYBE_UNUSED(tag);
        _loaders.erase(&loader);
    }

    void reload() noexcept {
        const std::lock_guard<decltype(_mutex)> lck{_mutex};
        if(!_loaders.empty()) {
            _open_configs.clear();
            for(auto loader : _loaders) {
                extract(loader).reload();
            }
        }
    }

private:
    auto _cat(const string_view l, const string_view r) noexcept
      -> const std::string& {
        return append_to(assign_to(_config_name, l), r);
    }

    auto _cat(
      const string_view a,
      const string_view b,
      const string_view c,
      const string_view d) noexcept -> const std::string& {
        return append_to(
          append_to(append_to(assign_to(_config_name, a), b), c), d);
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
        if(!cfg_path.empty()) {
            if(is_regular_file(cfg_path) || is_fifo(cfg_path)) {
                if(auto comp{_get_config(cfg_path)}) {
                    if(auto attr{comp.find(
                         basic_string_path(key, EAGINE_TAG(split_by), "."),
                         tags)}) {
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
            if(auto config_dir{main_context().user().config_dir_path()}) {
                config_path.append(std::string_view{extract(config_dir)});
                config_path.append("eagine");
                config_path.append(std::string_view{name});
            }
        } else if(kind == config_path_kind::system) {
            if(auto config_dir{main_context().system().config_dir_path()}) {
                config_path.append(std::string_view{extract(config_dir)});
                config_path.append("eagine");
                config_path.append(std::string_view{name});
            }
        } else if(kind == config_path_kind::install) {
            if(auto config_dir{main_context().build().config_dir_path()}) {
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
                return valtree::from_yaml_text(
                  memory::as_chars(content), *this);
            }
        } else if(cfg_path.extension() == ".json") {
            if(file_contents content{cfg_path.string()}) {
                return valtree::from_json_text(
                  memory::as_chars(content), *this);
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

    std::recursive_mutex _mutex;
    std::map<std::string, valtree::compound> _open_configs;
    std::set<application_config_value_loader*> _loaders;
    std::vector<string_view> _tag_list;
    std::string _config_name;
    std::string _hostname;
};
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto application_config::_impl() noexcept -> application_config_impl* {
    if(EAGINE_UNLIKELY(!_pimpl)) {
        try {
            _pimpl = std::make_shared<application_config_impl>(*this);
        } catch(...) {
        }
    }
    return _pimpl.get();
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
void application_config::link(
  const string_view key,
  const string_view tag,
  application_config_value_loader& loader) noexcept {
    if(const auto impl{_impl()}) {
        return extract(impl).link(key, tag, loader);
    }
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
void application_config::unlink(
  const string_view key,
  const string_view tag,
  application_config_value_loader& loader) noexcept {
    if(const auto impl{_impl()}) {
        return extract(impl).unlink(key, tag, loader);
    }
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
void application_config::reload() noexcept {
    if(const auto impl{_impl()}) {
        return extract(impl).reload();
    }
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto application_config::_find_comp_attr(
  const string_view key,
  const string_view tag) noexcept -> valtree::compound_attribute {
    if(const auto impl{_impl()}) {
        return extract(impl).find_compound_attribute(key, tag);
    }
    return {};
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto application_config::_prog_args() noexcept -> const program_args& {
    return main_context().args();
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto application_config::_prog_arg_name(string_view key) noexcept
  -> std::string {
    std::string arg_name;
    arg_name.reserve(std_size(3 + key.size()));
    arg_name.append("--");
    for(auto c : key) {
        if(c == '.' || c == '_') {
            c = '-';
        }
        arg_name.append(&c, 1U);
    }
    return arg_name;
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto application_config::_find_prog_arg(const string_view key) noexcept
  -> program_arg {
    return _prog_args().find(_prog_arg_name(key));
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto application_config::_eval_env_var(const string_view key) noexcept
  -> optionally_valid<string_view> {
    std::string arg_name;
    arg_name.reserve(std_size(7 + key.size()));
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
} // namespace eagine
