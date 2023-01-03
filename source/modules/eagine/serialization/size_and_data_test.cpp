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
import eagine.core.serialization;
import <vector>;
//------------------------------------------------------------------------------
void roundtrip_1(auto& s) {
    eagitest::case_ test{s, 1, "roundtrip 1"};
    auto& rg{test.random()};
    using eagine::are_equal;
    using eagine::cover;
    using eagine::view;

    std::vector<eagine::byte> buf;
    buf.resize(1024);
    eagine::memory::block dst{cover(buf)};
    for(unsigned i = 0; i < test.repeats(10000); ++i) {
        using eagine::get_data_with_size;
        using eagine::store_data_with_size;

        std::vector<eagine::byte> temp;
        temp.resize(rg.get_std_size(0, 1000));
        rg.fill(temp);

        const eagine::memory::const_block orig{view(temp)};
        const eagine::memory::block stor{store_data_with_size(orig, dst)};
        const eagine::memory::block back{get_data_with_size(stor)};

        test.check_equal(orig.empty(), back.empty(), "empty");
        test.check_equal(orig.size(), back.size(), "size");
        test.check(are_equal(orig, back), "content");
    }
}
//------------------------------------------------------------------------------
void for_each_1(auto& s) {
    eagitest::case_ test{s, 2, "for each 1"};
    eagitest::track trck{test, 0, 2};

    auto& rg{test.random()};
    using eagine::are_equal;
    using eagine::cover;
    using eagine::view;

    std::vector<eagine::byte> buf;
    buf.resize(16 * 1024);
    const eagine::memory::block dst{cover(buf)};

    for(unsigned i = 0; i < test.repeats(100); ++i) {
        using eagine::store_data_with_size;
        auto next = dst;

        std::vector<std::vector<eagine::byte>> temp;

        do {
            temp.emplace_back();
            auto& back = temp.back();
            back.resize(rg.get_std_size(0, 1000));
            rg.fill(back);

            const auto stor{store_data_with_size(view(back), next)};
            if(stor.empty()) {
                temp.pop_back();
                next = {};
            } else {
                next = skip(next, stor.size());
                trck.checkpoint(1);
            }
        } while(next);

        std::size_t count{0};
        eagine::for_each_data_with_size(
          dst, [&](eagine::memory::const_block b) {
              test.ensure(count < temp.size(), "index ok");
              test.check_equal(b.empty(), temp[count].empty(), "empty");
              test.check_equal(
                b.size(), eagine::span_size(temp[count].size()), "size");
              ++count;
              trck.checkpoint(2);
          });

        test.check_equal(count, temp.size(), "count ok");
    }
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "data_and_size", 2};
    test.once(roundtrip_1);
    test.once(for_each_1);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
