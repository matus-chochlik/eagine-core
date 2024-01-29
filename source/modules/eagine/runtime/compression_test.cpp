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
import eagine.core.runtime;
//------------------------------------------------------------------------------
void do_compress_roundtrip_default(
  auto& test,
  eagine::data_compression_level level) {
    auto& rg{test.random()};

    eagine::memory::buffer_pool buffers;
    eagine::data_compressor compress{buffers};

    for(unsigned r = 0; r < test.repeats(100); ++r) {
        auto orig{buffers.get(rg.get_span_size(r * 1024, 128 * 1024))};
        const eagine::memory::block orig_blk{orig};
        rg.fill_with_bytes(orig_blk);

        auto pckd{compress.default_compress(orig_blk, level)};

        auto copy{compress.default_decompress(eagine::memory::view(pckd))};
        const eagine::memory::const_block copy_blk{copy};

        test.check(eagine::are_equal(orig_blk, copy_blk), "same content");
    }
}
//------------------------------------------------------------------------------
void compress_roundtrip_default_none(auto& s) {
    eagitest::case_ test{s, 1, "round-trip default none"};
    do_compress_roundtrip_default(test, eagine::data_compression_level::none);
}
//------------------------------------------------------------------------------
void compress_roundtrip_default_lowest(auto& s) {
    eagitest::case_ test{s, 2, "round-trip default lowest"};
    do_compress_roundtrip_default(test, eagine::data_compression_level::lowest);
}
//------------------------------------------------------------------------------
void compress_roundtrip_default_normal(auto& s) {
    eagitest::case_ test{s, 3, "round-trip default normal"};
    do_compress_roundtrip_default(test, eagine::data_compression_level::normal);
}
//------------------------------------------------------------------------------
void compress_roundtrip_default_highest(auto& s) {
    eagitest::case_ test{s, 4, "round-trip default highest"};
    do_compress_roundtrip_default(
      test, eagine::data_compression_level::highest);
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "compression", 4};
    test.once(compress_roundtrip_default_none);
    test.once(compress_roundtrip_default_lowest);
    test.once(compress_roundtrip_default_normal);
    test.once(compress_roundtrip_default_highest);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
