/// @example eagine/user_info.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import std;

namespace eagine {

auto main(main_ctx& ctx) -> int {
    const std::string na{"N/A"};

    auto& usr = ctx.user();

    ctx.cio()
      .print("user", "login name: ${login}")
      .arg("login", usr.login_name().value_or(na));

    ctx.cio()
      .print("user", "home path: ${home}")
      .arg("home", usr.home_dir_path().value_or(na));

    return 0;
}

} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

