/// @example eagine/from_string.cpp
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
    using duration = std::chrono::duration<float, std::milli>;

    main_ctx_object out{identifier{"fromString"}, ctx};

    for(auto& arg : ctx.args()) {
        if(const auto opt_int{from_string<int>(arg)}) {
            out.cio_print("integer: ${value}")
              .arg(identifier{"value"}, extract(opt_int));
        } else if(const auto opt_float{from_string<float>(arg)}) {
            out.cio_print("float: ${value}")
              .arg(identifier{"value"}, extract(opt_float));
        } else if(const auto opt_double{from_string<double>(arg)}) {
            out.cio_print("double: ${value}")
              .arg(identifier{"value"}, extract(opt_double));
        } else if(const auto opt_char{from_string<char>(arg)}) {
            out.cio_print("character: ${value}")
              .arg(identifier{"value"}, extract(opt_char));
        } else if(const auto opt_bool{from_string<bool>(arg)}) {
            out.cio_print("boolean: ${value}")
              .arg(identifier{"value"}, extract(opt_bool));
        } else if(const auto opt_dur{from_string<duration>(arg)}) {
            out.cio_print("duration: ${value}")
              .arg(identifier{"value"}, extract(opt_dur));
        } else {
            out.cio_print("other: ${value}").arg(identifier{"value"}, arg.get());
        }
    }

    return 0;
}

} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

