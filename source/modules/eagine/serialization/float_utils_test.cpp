/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.serialization;
//------------------------------------------------------------------------------
template <typename T>
void roundtrip_1_T(eagitest::case_& test) {
    auto& rg{test.random()};

    const std::type_identity<T> tid{};
    for(unsigned i = 0; i < test.repeats(10000); ++i) {
        using eagine::float_utils::compose;
        using eagine::float_utils::decompose;
        const auto orig{rg.get_any<T>()};
        const auto conv{compose(decompose(orig, tid), tid)};

        test.check_close(orig, conv, "match");
    }
}
//------------------------------------------------------------------------------
void roundtrip_1_float(auto& s) {
    eagitest::case_ test{s, 1, "roundtrip 1 float"};
    roundtrip_1_T<float>(test);
}
//------------------------------------------------------------------------------
void roundtrip_1_double(auto& s) {
    eagitest::case_ test{s, 2, "roundtrip 1 double"};
    roundtrip_1_T<double>(test);
}
//------------------------------------------------------------------------------
void roundtrip_1_long_double(auto& s) {
    eagitest::case_ test{s, 3, "roundtrip 1 long double"};
    roundtrip_1_T<long double>(test);
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "float_utils", 3};
    test.once(roundtrip_1_float);
    test.once(roundtrip_1_double);
    test.once(roundtrip_1_long_double);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
