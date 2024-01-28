/// @example eagine/random_bytes.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import std;

namespace eagine {
//------------------------------------------------------------------------------
auto main(main_ctx&) -> int {
    using namespace eagine;

    std::vector<byte> bytes(256);
    fill_with_random_bytes(cover(bytes));

    std::cout << hexdump(view(bytes)) << std::endl;

    return 0;
}
//------------------------------------------------------------------------------
} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

