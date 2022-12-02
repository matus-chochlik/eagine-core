/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
//------------------------------------------------------------------------------
void base64_roundtrip(auto& s) {
    eagitest::case_ test{s, 1, "roundtrip"};
    auto& rg{test.random()};

    using namespace eagine;

    std::vector<byte> orig;
    std::vector<byte> deco;
    std::string enco;

    for(unsigned i = 0; i < test.repeats(10000); ++i) {

        orig.resize(rg.get_std_size(0, 10000));
        rg.fill(orig);

        auto res = bool(base64_encode(view(orig), enco));
        test.check(res, "encode result");
        if(res) {
            res = bool(base64_decode(view(enco), deco));
            test.check(res, "decode result");
            if(res) {
                test.check(orig == deco, "decoded matches original");
            }
        }
    }
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "base64", 1};
    test.once(base64_roundtrip);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
