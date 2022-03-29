/// @example eagine/application_config.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/app_config.hpp>
#include <eagine/console/console.hpp>
#include <eagine/main.hpp>
#include <chrono>
#include <thread>

namespace eagine {

auto main(main_ctx& ctx) -> int {
    const std::string na{"N/A"};

    auto& cfg = ctx.config();
    std::string s;
    int i;
    std::vector<float> v;

    if(cfg.fetch("value_1", s)) {
        cfg.cio_print("string: ${s}").arg(EAGINE_ID(s), s);
    }

    if(cfg.fetch("value_2", s)) {
        cfg.cio_print("string: ${s}").arg(EAGINE_ID(s), s);
    }

    if(cfg.fetch("section_a.subsection_b.value_c", i)) {
        cfg.cio_print("integer : ${i}").arg(EAGINE_ID(i), i);
    }

    if(cfg.fetch("section_a.subsection_b.values", v)) {
        for(auto e : v) {
            cfg.cio_print("float : ${f}").arg(EAGINE_ID(f), e);
        }
    }

    const application_reconfig_value<std::chrono::duration<float>> delay{
      cfg, "section_a.subsection_b.delay", std::chrono::seconds(1)};
    const auto start{std::chrono::steady_clock::now()};
    while(true) {
        const auto elapsed = std::chrono::steady_clock::now() - start;
        if(elapsed > delay.value()) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
        cfg.reload();
    }

    return 0;
}

} // namespace eagine
