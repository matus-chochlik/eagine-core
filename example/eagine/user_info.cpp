/// @example eagine/user_info.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#if EAGINE_CORE_MODULE
import eagine.core;
import <iostream>;
#else
#include <eagine/console/console.hpp>
#include <eagine/main_ctx.hpp>
#include <eagine/user_info.hpp>
#include <iostream>
#endif

namespace eagine {

auto main(main_ctx& ctx) -> int {
    const std::string na{"N/A"};

    auto& usr = ctx.user();

    ctx.cio()
      .print(identifier{"user"}, "login name: ${login}")
      .arg(identifier{"login"}, extract_or(usr.login_name(), na));

    ctx.cio()
      .print(identifier{"user"}, "home path: ${home}")
      .arg(identifier{"home"}, extract_or(usr.home_dir_path(), na));

    return 0;
}

} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

