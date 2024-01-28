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
import eagine.core.string;
//------------------------------------------------------------------------------
void default_keyboard_distance(auto& s) {
    eagitest::case_ test{s, 1, "default"};
    eagitest::track trck{test, 0, 1};

    const std::string_view keys{
      "\\\"'`~1!2@3#4$5%6^7&8*9(0)-_=+qQwWeErRtTyYuUiIoOpP[{]}|"
      "aAsSdDfFgGhHjJkKlL;:zZxXcCvVbBnNmM,<.>/?"};

    const eagine::default_keyboard_layout l;
    const eagine::keyboard_distance kbd{l};

    for(const auto k1 : keys) {
        const auto v1{eagine::memory::view_one(k1)};
        for(const auto k2 : keys) {
            const auto v2{eagine::memory::view_one(k2)};
            if(kbd(v1, v2) > 0.F != (k1 != k2)) {
                std::cout << k1 << "|" << k2 << std::endl;
            }
            test.check_equal(kbd(v1, v2) > 0.F, k1 != k2, "!=");
            test.check_equal(kbd(v1, v2) <= 0.F, k1 == k2, "==");
            trck.checkpoint(1);
        }
    }
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "keyboard_distance", 1};
    test.once(default_keyboard_distance);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
