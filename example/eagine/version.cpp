/// @example eagine/version.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/build_info.hpp>
#include <eagine/console/console.hpp>
#include <eagine/main.hpp>

namespace eagine {

auto main(main_ctx& ctx) -> int {

    const auto& bi = ctx.build();

    const auto [major, minor, patch, commit] = bi.version_tuple();
    ctx.cio()
      .print(EAGINE_ID(version), "${major}.${minor}.${patch}-${commit}")
      .arg(EAGINE_ID(major), major)
      .arg(EAGINE_ID(minor), minor)
      .arg(EAGINE_ID(patch), patch)
      .arg(EAGINE_ID(commit), commit);

    if(bi.version_at_least(0, 37, 2, 15)) {
        ctx.cio().print(EAGINE_ID(version), "version recent enough");
    }

    return 0;
}

} // namespace eagine
