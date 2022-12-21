/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#if __has_include(<pwd.h>)
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#ifndef EAGINE_POSIX
#define EAGINE_POSIX 1
#endif
#else
#ifndef EAGINE_POSIX
#define EAGINE_POSIX 0
#endif
#endif

module eagine.core.main_ctx;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.valid_if;
import <array>;
import <memory>;
import <optional>;
import <string>;

namespace eagine {
//------------------------------------------------------------------------------
class user_info_impl {
public:
    user_info_impl() {}

    const std::string login_name{_get_username()};
    const std::string home_dir_path{_get_home_dir_path()};
    const std::string config_dir_path{_get_config_dir_path()};

private:
    static auto _get_username() -> std::string {
#if EAGINE_POSIX
        if(const auto pw{::getpwuid(::getuid())}) {
            return {extract(pw).pw_name};
        }
        std::array<char, 128> temp{};
        if(::getlogin_r(temp.data(), temp.size())) {
            return {temp.data(), temp.size()};
        }
#endif
        if(const auto opt_var{get_environment_variable("USER")}) {
            return to_string(extract(opt_var));
        }
        return {};
    }

    static auto _get_home_dir_path() -> std::string {
#if EAGINE_POSIX
        if(const auto pw{::getpwuid(::getuid())}) {
            return {extract(pw).pw_dir};
        }
#endif
        if(const auto opt_var{get_environment_variable("HOME")}) {
            return to_string(extract(opt_var));
        }
        return {};
    }

    static auto _get_config_dir_path() -> std::string {
        auto result = _get_home_dir_path();
        if(not result.empty() and result.back() != '/') {
            result.append("/");
        }
        result.append(".config");
        return result;
    }
};
//------------------------------------------------------------------------------
auto user_info::_impl() noexcept -> user_info_impl* {
    if(not _pimpl) [[unlikely]] {
        try {
            _pimpl = std::make_shared<user_info_impl>();
        } catch(...) {
        }
    }
    return _pimpl.get();
}
//------------------------------------------------------------------------------
auto user_info::login_name() noexcept -> valid_if_not_empty<string_view> {
    if(const auto impl{_impl()}) {
        return {extract(impl).login_name};
    }
    return {};
}
//------------------------------------------------------------------------------
auto user_info::home_dir_path() noexcept -> valid_if_not_empty<string_view> {
    if(const auto impl{_impl()}) {
        return {extract(impl).home_dir_path};
    }
    return {};
}
//------------------------------------------------------------------------------
auto user_info::config_dir_path() noexcept -> valid_if_not_empty<string_view> {
    if(const auto impl{_impl()}) {
        return {extract(impl).config_dir_path};
    }
    return {};
}
//------------------------------------------------------------------------------
} // namespace eagine
