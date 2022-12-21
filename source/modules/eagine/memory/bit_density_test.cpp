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
//------------------------------------------------------------------------------
void bit_density_length_trip(unsigned, auto& s) {
    using namespace eagine;

    eagitest::case_ test{s, 1, "default construct"};
    auto& rg{test.random()};

    for(span_size_t bits = 1; bits <= byte_bits(); ++bits) {

        const auto orig_size{rg.get_between<span_size_t>(0, 10000)};
        const auto thin_size{dissolved_bits_length(orig_size, bits)};
        const auto comp_size{concentrated_bits_length(thin_size, bits)};

        test.check(
          ((thin_size > orig_size) == (bits < byte_bits())) or
            ((thin_size == orig_size) == (orig_size == 0)),
          "A");
        test.check(thin_size >= 0, "B");
        test.check(comp_size - orig_size >= 0, "C");
        test.check(comp_size - orig_size <= 1, "D");
    }
}
//------------------------------------------------------------------------------
void bit_density_trip(unsigned, auto& s) {
    using namespace eagine;

    eagitest::case_ test{s, 2, "default construct"};
    auto& rg{test.random()};

    std::vector<byte> orig;
    std::vector<byte> thin;
    std::vector<byte> comp;
    bool res = false;

    orig.resize(rg.get_between<std::size_t>(0, 10000));
    rg.fill(orig);
    for(span_size_t bits = 1; bits <= byte_bits(); ++bits) {
        thin.resize(
          std::size_t(dissolved_bits_length(span_size_t(orig.size()), bits)));

        span_size_t inp = 0;
        span_size_t out = 0;

        res = do_dissolve_bits(
          make_span_getter(inp, view(orig)),
          make_span_putter(out, cover(thin)),
          bits);

        test.check(res, "dissolve result");
        if(res) {
            comp.resize(std::size_t(
              concentrated_bits_length(span_size_t(thin.size()), bits)));

            inp = 0;
            out = 0;

            res = do_concentrate_bits(
              make_span_getter(inp, view(thin)),
              make_span_putter(out, cover(comp)),
              bits);
            test.check(res, "concentrate result");
            if(res) {
                test.check(
                  are_equal(view(orig), head(view(comp), out)), "equal");
            }
        }
    }
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "bit_density", 2};
    test.repeat(100, bit_density_length_trip);
    test.repeat(100, bit_density_trip);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
