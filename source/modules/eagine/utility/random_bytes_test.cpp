/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.types;
import eagine.core.memory;
import eagine.core.utility;
//------------------------------------------------------------------------------
void random_bytes_1(auto& s) {
    eagitest::case_ test{s, 1, "1"};
    using namespace eagine;

    const std::size_t samples = 1024U * 1024U * 32U;
    const std::size_t options = 1U << 8U;
    const std::size_t mean = samples / options;

    std::vector<unsigned char> buffer(samples);
    std::vector<std::size_t> counts(options, 0);

    fill_with_random_bytes(cover(buffer));

    for(auto b : buffer) {
        ++counts[b];
    }

    for(auto c : counts) {
        double x = (int(c) - int(mean)) / double(mean);
        test.check_close(x, 0.0, 0.05, "zero");
    }
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "random_bytes", 1};
    test.once(random_bytes_1);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
