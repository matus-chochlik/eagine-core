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
void multi_byte_seq_1(auto& s) {
    using namespace eagine;
    eagitest::case_ test{s, "seq 1"};
    auto& rg{s.random()};

    std::vector<byte> bytes;

    multi_byte::code_point_t cp = 0, cp2;

    for(span_size_t l = 1; l <= 6; ++l) {

        bytes.resize(std_size(l));
        while(cp < multi_byte::max_code_point(l).value()) {

            test.check(
              bool(multi_byte::encode_code_point(cp, cover(bytes))), "encode");
            test.check(
              multi_byte::is_valid_encoding(
                multi_byte::make_cbyte_span(view(bytes))),
              "is valid");

            cp2 = multi_byte::decode_code_point(
                    multi_byte::make_cbyte_span(view(bytes)))
                    .value();

            test.check_equal(cp, cp2, "code points equal");

            cp += rg.get_between(
              multi_byte::code_point_t(1),
              multi_byte::code_point_t(l * l * l * l),
              std::type_identity<multi_byte::code_point_t>{});
        }
    }
}
//------------------------------------------------------------------------------
void multi_byte_seq_2(auto& s) {
    using namespace eagine;
    eagitest::case_ test{s, "seq 2"};
    auto& rg{s.random()};

    std::vector<multi_byte::code_point> cps, cps2;
    std::vector<byte> bytes;

    // TODO: repeat
    for(int i = 0; i < 10000; ++i) {

        std::size_t len = rg.get_between(std::size_t(1), std::size_t(100));
        cps.resize(len);

        for(multi_byte::code_point cp : cps) {
            do {
                cp = rg.get_any(std::type_identity<multi_byte::code_point_t>{});
            } while(!cp.is_valid());
        }

        bytes.resize(
          std_size(multi_byte::encoding_bytes_required(view(cps)).value()));

        test.check(
          bool(multi_byte::encode_code_points(view(cps), cover(bytes))),
          "encode");

        test.ensure(
          multi_byte::is_valid_encoding(
            multi_byte::make_cbyte_span(view(bytes))),
          "is valid");

        cps2.resize(std_size(multi_byte::decoding_code_points_required(
                               multi_byte::make_cbyte_span(view(bytes)))
                               .value()));

        test.ensure(cps.size() == cps2.size(), "equal size");

        test.check(
          bool(multi_byte::decode_code_points(
            multi_byte::make_cbyte_span(view(bytes)), cover(cps2))),
          "decode");

        test.check(are_equal(view(cps), view(cps2)), "equal encoding");
    }
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "multi_byte"};
    test.once(multi_byte_seq_1);
    test.once(multi_byte_seq_2);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
