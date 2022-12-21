/// @example eagine/hexdump.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import <iostream>;

namespace eagine {

auto main(main_ctx& ctx) -> int {
    for(const auto& arg : ctx.args()) {
        if(not arg.starts_with("-")) {
            if(arg.prev().is_tag("-f", "--file")) {
                file_contents fc(arg.get());
                std::cout << arg << '|' << hexdump(fc.block()) << std::endl;
            } else if(arg.prev().is_tag("-s", "--string")) {
                std::cout << arg << '|' << hexdump(arg.block()) << std::endl;
            }
        }
    }

    span_size_t i = 0;
    const auto get = [&]() -> std::optional<byte> {
        if(i < 256) {
            return {byte(i++)};
        }
        return {};
    };
    const auto put = [](const char c) {
        std::cout << c;
        return true;
    };
    hexdump::apply({construct_from, get}, {construct_from, put});

    return 0;
}
} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

