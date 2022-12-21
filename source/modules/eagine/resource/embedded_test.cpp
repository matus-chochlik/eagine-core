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
import eagine.core.runtime;
import eagine.core.resource;
//------------------------------------------------------------------------------
void embed_self(auto& s) {
    using namespace eagine;
    eagitest::case_ test{s, 1, "embed self"};
    eagitest::track trck{test, 0, 1};
    const auto self{embed<"self">(__FILE__)};

    test.check(bool(self), "is true");
    test.check(self.format() == embedded_resource_format::cpp, "format");
    test.check(not self.source_path().empty(), "non empty path");
    test.check(not self.embedded_block().empty(), "non empty content");
    test.check(
      are_equal(memory::const_block(self), self.embedded_block()),
      "same content");

    memory::buffer_pool bufs;
    data_compressor comp{data_compression_method::none, bufs};
    memory::buffer buf1;
    self.append_to(comp, buf1);
    test.check(
      are_equal(self.embedded_block(), view(buf1)), "appended content");

    memory::buffer buf2;
    self.unpack(comp, buf2);
    test.check(are_equal(view(buf1), view(buf2)), "unpacked content");

    memory::buffer buf3;
    self.fetch(
      comp,
      data_compressor::data_handler{
        construct_from, [&](const memory::const_block blk) {
            trck.passed_part(1);
            memory::append_to(blk, buf3);
            return true;
        }});
    test.check(are_equal(view(buf2), view(buf3)), "fetched content");
}
//------------------------------------------------------------------------------
void search_self(auto& s) {
    using namespace eagine;
    eagitest::case_ test{s, 2, "search self"};
    const auto self{search_resource("self")};

    test.check(bool(self), "is true");
    test.check(self.format() == embedded_resource_format::cpp, "format");
    test.check(not self.source_path().empty(), "non empty path");
    test.check(not self.embedded_block().empty(), "non empty content");
    test.check(
      are_equal(memory::const_block(self), self.embedded_block()),
      "same content");
}
//------------------------------------------------------------------------------
void search_cmake(auto& s) {
    using namespace eagine;
    eagitest::case_ test{s, 3, "search cmake"};
    eagitest::track trck{test, 0, 1};
    const auto res{search_resource("CMakeLists")};

    test.check(bool(res), "is true");
    test.check(res.format() == embedded_resource_format::text_utf8, "format");
    test.check(not res.source_path().empty(), "non empty path");
    test.check(not res.embedded_block().empty(), "non empty content");
    test.check(
      are_equal(memory::const_block(res), res.embedded_block()),
      "same content");

    memory::buffer_pool bufs;
    data_compressor comp{data_compression_method::none, bufs};
    memory::buffer buf1;
    res.append_to(comp, buf1);
    test.check(are_equal(res.embedded_block(), view(buf1)), "appended content");

    memory::buffer buf2;
    res.unpack(comp, buf2);
    test.check(are_equal(view(buf1), view(buf2)), "unpacked content");

    memory::buffer buf3;
    res.fetch(
      comp,
      data_compressor::data_handler{
        construct_from, [&](const memory::const_block blk) {
            trck.passed_part(1);
            memory::append_to(blk, buf3);
            return true;
        }});
    test.check(are_equal(view(buf2), view(buf3)), "fetched content");
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "embedded", 3};
    test.once(embed_self);
    test.once(search_self);
    test.once(search_cmake);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
