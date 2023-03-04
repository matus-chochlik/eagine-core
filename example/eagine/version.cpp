/// @example eagine/version.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import std;

namespace eagine {

auto main(main_ctx& ctx) -> int {

    const auto& vi = ctx.version();

    const auto [major, minor, patch, commit] = vi.version_tuple();
    ctx.cio()
      .print(identifier{"version"}, "${major}.${minor}.${patch}-${commit}")
      .arg(identifier{"major"}, major)
      .arg(identifier{"minor"}, minor)
      .arg(identifier{"patch"}, patch)
      .arg(identifier{"commit"}, commit);

    if(vi.version_at_least(0, 37, 2, 15)) {
        ctx.cio().print(identifier{"version"}, "version recent enough");
    }

    return 0;
}

} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

