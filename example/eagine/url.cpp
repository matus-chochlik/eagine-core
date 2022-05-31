/// @example eagine/url.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/console/console.hpp>
#include <eagine/main.hpp>
#include <eagine/main_ctx_object.hpp>
#include <eagine/url.hpp>

namespace eagine {

auto main(main_ctx& ctx) -> int {
    const string_view n{"N/A"};

    main_ctx_object out{EAGINE_ID(URL), ctx};

    std::string line;
    while((std::cin >> line).good()) {
        url u{line};

        out.cio_print("scheme:     ${value}")
          .arg(EAGINE_ID(value), either_or(u.scheme(), n));
        out.cio_print("login:      ")
          .arg(EAGINE_ID(value), either_or(u.login(), n));
        out.cio_print("password:   ")
          .arg(EAGINE_ID(value), either_or(u.password(), n));
        out.cio_print("host:       ")
          .arg(EAGINE_ID(value), either_or(u.host(), n));
        out.cio_print("port:       ")
          .arg(EAGINE_ID(value), either_or(u.port(), n));
        out.cio_print("path:       ")
          .arg(EAGINE_ID(value), either_or(u.path_str(), n));

        const auto entry_cio{out.cio_print("path entries:").to_be_continued()};

        for(auto& entry : u.path()) {
            entry_cio.print("'${value}'").arg(EAGINE_ID(value), entry);
        }
        out.cio_print("query:      ")
          .arg(EAGINE_ID(value), either_or(u.query_str(), n));

        const auto arg_cio{out.cio_print("query arguments:").to_be_continued()};

        for(auto& [name, value] : u.query()) {
            arg_cio.print("${name}: '${value}'")
              .arg(EAGINE_ID(name), name)
              .arg(EAGINE_ID(value), value);
        }
        out.cio_print("fragment:   ")
          .arg(EAGINE_ID(value), either_or(u.fragment(), n));
    }

    return 0;
}

} // namespace eagine
