/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

module eagine.core.runtime;
import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.valid_if;

namespace eagine {
//------------------------------------------------------------------------------
auto basic_config::program_arg_name(string_view key, string_view) const noexcept
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
auto basic_config::environment_var_name(string_view key, string_view)
  const noexcept -> std::string {
    std::string var_name;
    var_name.reserve(integer(safe_add(7, key.size())));
    var_name.append("EAGINE_");
    for(auto c : key) {
        if(c == '.' or c == '-') {
            c = '_';
        } else {
            c = static_cast<char>(std::toupper(c));
        }
        var_name.append(&c, 1U);
    }
    return var_name;
}
//------------------------------------------------------------------------------
auto basic_config::find_program_arg(string_view key, const string_view tag)
  const noexcept -> program_arg {
    return _args.find(program_arg_name(key, tag));
}
//------------------------------------------------------------------------------
auto basic_config::eval_environment_var(string_view key, const string_view tag)
  -> optionally_valid<string_view> {
    return get_environment_variable(environment_var_name(key, tag));
}
//------------------------------------------------------------------------------
auto basic_config::is_set(const string_view key, const string_view tag) noexcept
  -> bool {
    const auto find_env_var{[=, this] {
        return eval_environment_var(key, tag);
    }};
    const auto return_false{[] {
        return optionally_valid<string_view>{{"false"}};
    }};

    return find_program_arg(key, tag)
             .next()
             .or_else(find_env_var)
             .or_else(return_false)
             .and_then(from_string<bool>(_1))
             .value_or(true) != false;
}
//------------------------------------------------------------------------------
auto basic_config::fetch_string(
  const string_view key,
  const string_view tag,
  std::string& dest) noexcept -> bool {
    const auto find_env_var{[=, this] {
        return get_environment_variable(environment_var_name(key, tag));
    }};

    return find_program_arg(key, tag)
      .next()
      .or_else(find_env_var)
      .and_then(assign_if_fits(_1, dest))
      .value_or(false);
}
//------------------------------------------------------------------------------
} // namespace eagine
