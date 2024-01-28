/// @example eagine/application_config.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import std;

namespace eagine {

auto main(main_ctx& ctx) -> int {
    const std::string na{"N/A"};

    auto& cfg = ctx.config();
    main_ctx_object out{"config", ctx};
    std::string s;
    int i{0};
    std::vector<float> v;

    if(cfg.fetch("value_1", s)) {
        out.cio_print("string: ${s}").arg("s", s);
    }

    if(cfg.fetch("value_2", s)) {
        out.cio_print("string: ${s}").arg("s", s);
    }

    if(cfg.fetch_string("section_a.secret_value_b", s)) {
        out.cio_print("secret: ${s}").arg("s", s);
    }

    if(cfg.fetch("section_a.subsection_b.value_c", i)) {
        out.cio_print("integer : ${i}").arg("i", i);
    }

    if(cfg.fetch("section_a.subsection_b.values", v)) {
        for(auto e : v) {
            out.cio_print("float : ${f}").arg("f", e);
        }
    }

    const application_reconfig_value<std::chrono::duration<float>> delay{
      cfg, "section_a.subsection_b.delay", std::chrono::seconds(1)};
    const auto start{std::chrono::steady_clock::now()};
    while(true) {
        const auto elapsed{std::chrono::steady_clock::now() - start};
        if(elapsed > delay.value()) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
        cfg.reload();
    }

    return 0;
}

} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

