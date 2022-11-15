/// @example eagine/build_info.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import <string>;

namespace eagine {

auto main(main_ctx& ctx) -> int {
    const std::string na{"N/A"};

    auto& compiler = ctx.compiler();
    auto& build = ctx.build();

    const identifier cioid{"build"};

    ctx.cio()
      .print(cioid, "architecture: ${arch}")
      .arg(identifier{"arch"}, either_or(compiler.architecture_name(), na));
    ctx.cio()
      .print(cioid, "compiler name: ${name}")
      .arg(identifier{"name"}, either_or(compiler.name(), na));

    ctx.cio()
      .print(cioid, "compiler version: ${major}.${minor}.${patch}")
      .arg(identifier{"major"}, either_or(compiler.version_major(), na))
      .arg(identifier{"minor"}, either_or(compiler.version_minor(), na))
      .arg(identifier{"patch"}, either_or(compiler.version_patch(), na));

    ctx.cio()
      .print(cioid, "install prefix: ${path}")
      .arg(
        identifier{"path"},
        identifier{"FsPath"},
        either_or(build.install_prefix(), na));

    return 0;
}

} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

