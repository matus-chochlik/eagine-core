/// @example eagine/user_info.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/console/console.hpp>
#include <eagine/main.hpp>
#include <eagine/user_info.hpp>
#include <iostream>

namespace eagine {

auto main(main_ctx& ctx) -> int {
    const std::string na{"N/A"};

    auto& usr = ctx.user();

    ctx.cio()
      .print(EAGINE_ID(user), "login name: ${login}")
      .arg(EAGINE_ID(login), extract_or(usr.login_name(), na));

    ctx.cio()
      .print(EAGINE_ID(user), "home path: ${home}")
      .arg(EAGINE_ID(home), extract_or(usr.home_dir_path(), na));

    return 0;
}

} // namespace eagine
