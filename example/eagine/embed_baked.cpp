/// @example eagine/embed_baked.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/embed.hpp>
#include <eagine/hexdump.hpp>
#include <eagine/main_ctx.hpp>
#include <iostream>

namespace eagine {

auto main(main_ctx& ctx) -> int {

    const auto res{embed(EAGINE_ID(ZebraFur), "zebra_fur")};
    std::cout << hexdump(res.unpack(ctx));

    return 0;
}

} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

