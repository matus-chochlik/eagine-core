/// @example eagine/url.cpp
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
    const string_view n{"N/A"};

    main_ctx_object out{"URL", ctx};

    std::string line;
    while((std::cin >> line).good()) {
        const url u{line};

        out.cio_print("hash-id:         ${value}").arg("value", u.hash_id());

        out.cio_print("scheme:          ${value}")
          .arg("value", either_or(u.scheme(), n));
        out.cio_print("login:           ${value}")
          .arg("value", either_or(u.login(), n));
        out.cio_print("password:        ${value}")
          .arg("value", either_or(u.password(), n));
        out.cio_print("host:            ${value}")
          .arg("value", either_or(u.host(), n));
        out.cio_print("port:            ${value}")
          .arg("value", either_or(u.port(), n));
        out.cio_print("path:            ${value}")
          .arg("value", either_or(u.path_str(), n));
        out.cio_print("path dirname:    ${value}")
          .arg("value", either_or(u.path_dirname(), n));
        out.cio_print("path basename:   ${value}")
          .arg("value", either_or(u.path_basename(), n));
        out.cio_print("path identifier: ${value}")
          .arg("value", u.path_identifier());

        const auto entry_cio{out.cio_print("path entries:").to_be_continued()};

        for(auto& entry : u.path()) {
            entry_cio.print("'${value}'").arg("value", entry);
        }
        out.cio_print("query:           ${value}")
          .arg("value", either_or(u.query_str(), n));

        const auto arg_cio{out.cio_print("query arguments:").to_be_continued()};

        for(auto& [name, value] : u.query()) {
            arg_cio.print("${name}: '${value}'")
              .arg("name", name)
              .arg("value", value);
        }
        out.cio_print("fragment:        ${value}")
          .arg("value", either_or(u.fragment(), n));
    }

    return 0;
}

} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

