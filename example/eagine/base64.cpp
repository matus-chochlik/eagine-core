/// @example eagine/base64.cpp
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
    std::string na{"-"};

    std::string temp;
    for(auto& arg : ctx.args()) {
        if(not arg.starts_with("-")) {
            if(arg.prev().is_tag("-f", "--file")) {
                file_contents fc(arg.get());
                ctx.cio()
                  .print("base64", "${src}|${enc}")
                  .arg("src", arg.get())
                  .arg("enc", base64_encode(fc.block(), temp).value_or(na));
            } else if(arg.prev().is_tag("-s", "--string")) {
                ctx.cio()
                  .print("base64", "${src}|${enc}")
                  .arg("src", arg.get())
                  .arg("enc", base64_encode(arg.block(), temp).value_or("-"));
            }
        }
    }
    return 0;
}

} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}
