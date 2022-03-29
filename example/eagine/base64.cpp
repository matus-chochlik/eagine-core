/// @example eagine/base64.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/base64.hpp>
#include <eagine/console/console.hpp>
#include <eagine/file_contents.hpp>
#include <eagine/main.hpp>
#include <eagine/program_args.hpp>

namespace eagine {

auto main(main_ctx& ctx) -> int {
    std::string na{"-"};

    std::string temp;
    for(auto& arg : ctx.args()) {
        if(!arg.starts_with("-")) {
            if(arg.prev().is_tag("-f", "--file")) {
                file_contents fc(arg.get());
                ctx.cio()
                  .print(EAGINE_ID(base64), "${src}|${enc}")
                  .arg(EAGINE_ID(src), arg.get())
                  .arg(
                    EAGINE_ID(enc),
                    extract_or(base64_encode(fc.block(), temp), na));
            } else if(arg.prev().is_tag("-s", "--string")) {
                ctx.cio()
                  .print(EAGINE_ID(base64), "${src}|${enc}")
                  .arg(EAGINE_ID(src), arg.get())
                  .arg(
                    EAGINE_ID(enc),
                    extract_or(base64_encode(arg.block(), temp), "-"));
            }
        }
    }
    return 0;
}

} // namespace eagine
