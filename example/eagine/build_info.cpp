/// @example eagine/build_info.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/build_info.hpp>
#include <eagine/compiler_info.hpp>
#include <eagine/console/console.hpp>
#include <eagine/main.hpp>

namespace eagine {

auto main(main_ctx& ctx) -> int {
    const std::string na{"N/A"};

    auto& compiler = ctx.compiler();
    auto& build = ctx.build();

    ctx.cio()
      .print(EAGINE_ID(build), "architecture: ${arch}")
      .arg(EAGINE_ID(arch), either_or(compiler.architecture_name(), na));
    ctx.cio()
      .print(EAGINE_ID(build), "compiler name: ${name}")
      .arg(EAGINE_ID(name), either_or(compiler.name(), na));

    ctx.cio()
      .print(EAGINE_ID(build), "compiler version: ${major}.${minor}.${patch}")
      .arg(EAGINE_ID(major), either_or(compiler.version_major(), na))
      .arg(EAGINE_ID(minor), either_or(compiler.version_minor(), na))
      .arg(EAGINE_ID(patch), either_or(compiler.version_patch(), na));

    ctx.cio()
      .print(EAGINE_ID(build), "build version: ${major}.${minor}.${patch}")
      .arg(EAGINE_ID(major), either_or(build.version_major(), na))
      .arg(EAGINE_ID(minor), either_or(build.version_minor(), na))
      .arg(EAGINE_ID(patch), either_or(build.version_patch(), na));

    ctx.cio()
      .print(EAGINE_ID(build), "install prefix: ${path}")
      .arg(
        EAGINE_ID(path),
        EAGINE_ID(FsPath),
        either_or(build.install_prefix(), na));

    return 0;
}

} // namespace eagine
