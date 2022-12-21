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
template <bool is_const, typename Case>
void memory_block_1(Case& test) {

    eagine::memory::basic_block<is_const> bmb;

    test.check(not bool(bmb), "is false");
    test.check(not bmb, "is not true");
    test.check(bmb.empty(), "is empty");
    test.check_equal(bmb.size(), 0, "size is zero");
    test.check(bmb.begin() == bmb.end(), "begin == end");
}
//------------------------------------------------------------------------------
void block_1(auto& s) {
    eagitest::case_ test{s, 1, "1"};
    memory_block_1<true>(test);
    memory_block_1<false>(test);
}
//------------------------------------------------------------------------------
template <bool is_const, typename Case>
void memory_block_2(Case& test) {
    using namespace eagine;

    memory::basic_block<false> bmb1;
    memory::basic_block<is_const> bmb2 = bmb1;

    test.check(not bool(bmb2), "is false");
    test.check(not bmb2, "is not true");
    test.check(bmb2.empty(), "is empty");
    test.check_equal(bmb2.size(), 0, "size is zero");
    test.check(bmb2.begin() == bmb2.end(), "begin == end");
}
//------------------------------------------------------------------------------
void block_2(auto& s) {
    eagitest::case_ test{s, 2, "2"};
    memory_block_2<true>(test);
    memory_block_2<false>(test);
}
//------------------------------------------------------------------------------
template <bool is_const, typename Case>
void memory_block_3(Case& test) {
    eagitest::track trck{test, 0, 1};
    using namespace eagine;

    int x;

    memory::basic_block<is_const> bmb = as_bytes(cover_one(x));

    test.check(bool(bmb), "is true");
    test.check(!!bmb, "is not false");
    test.check(not bmb.empty(), "is not empty");
    test.check_equal(bmb.size(), span_size(sizeof(x)), "size is ok");
    test.check(bmb.begin() != bmb.end(), "begin != end");

    span_size_t s = 0;

    for([[maybe_unused]] byte b : bmb) {
        ++s;
        trck.passed_part(1);
    }

    test.check_equal(bmb.size(), s, "size is ok");
}
//------------------------------------------------------------------------------
void block_3(auto& s) {
    eagitest::case_ test{s, 3, "3"};
    memory_block_3<true>(test);
    memory_block_3<false>(test);
}
//------------------------------------------------------------------------------
template <bool is_const, typename Case>
void memory_block_4(Case& test) {
    eagitest::track trck{test, 0, 1};
    using namespace eagine;

    double x[42];

    memory::basic_block<is_const> bmb = as_bytes(cover(x));

    test.check(bool(bmb), "is true");
    test.check(!!bmb, "is not false");
    test.check(not bmb.empty(), "is not empty");
    test.check_equal(bmb.size(), span_size(sizeof(x)), "size is ok");
    test.check(bmb.begin() != bmb.end(), "begin != end");

    span_size_t s = 0;

    for([[maybe_unused]] byte b : bmb) {
        ++s;
        trck.passed_part(1);
    }

    test.check_equal(bmb.size(), s, "size is ok");
}
//------------------------------------------------------------------------------
void block_4(auto& s) {
    eagitest::case_ test{s, 4, "4"};
    memory_block_4<true>(test);
    memory_block_4<false>(test);
}
//------------------------------------------------------------------------------
template <bool is_const, typename Case>
void memory_block_5(Case& test) {
    eagitest::track trck{test, 0, 1};
    auto& rg{test.random()};
    using namespace eagine;

    byte x[256];

    for(byte& b : x) {
        b = rg.get_byte(0x00, 0xFF);
    }

    memory::basic_block<is_const> bmb = as_bytes(memory::cover(x));

    test.check(bool(bmb), "is true");
    test.check(!!bmb, "is not false");
    test.check(not bmb.empty(), "is not empty");
    test.check_equal(bmb.size(), span_size(sizeof(x)), "size is ok");
    test.check(bmb.begin() != bmb.end(), "begin != end");

    for(span_size_t i = 0; i < bmb.size(); ++i) {
        test.check_equal(bmb.data()[i], x[i], "indexing is ok");
        trck.passed_part(1);
    }
}
//------------------------------------------------------------------------------
void block_5(auto& s) {
    eagitest::case_ test{s, 5, "5"};
    memory_block_5<true>(test);
    memory_block_5<false>(test);
}
//------------------------------------------------------------------------------
template <bool is_const, typename Case>
void memory_block_6(Case& test) {
    using namespace eagine;

    unsigned x[10];

    memory::basic_block<is_const> bmb1 = as_bytes(memory::cover(x));

    test.check(bool(bmb1), "is true");
    test.check(!!bmb1, "is not false");
    test.check(not bmb1.empty(), "is not empty");
    test.check_equal(bmb1.size(), span_size(sizeof(x)), "size is ok");

    memory::basic_block<is_const> bmb2(std::move(bmb1));

    test.check(not bmb2.empty(), "is not empty");

    memory::basic_block<is_const> bmb3;

    test.check(bmb3.empty(), "is empty");

    bmb3 = std::move(bmb2);

    test.check(not bmb3.empty(), "is not empty");

    memory::basic_block<is_const> bmb4 = as_bytes(cover(x));

    test.check(are_equal(bmb1, bmb2), "are equal 1");
    test.check(are_equal(bmb3, bmb4), "are equal 2");
}
//------------------------------------------------------------------------------
void block_6(auto& s) {
    eagitest::case_ test{s, 6, "6"};
    memory_block_6<true>(test);
    memory_block_6<false>(test);
}
//------------------------------------------------------------------------------
template <typename T, typename Case>
void memory_block_7(Case& test) {
    auto& rg{test.random()};
    using namespace eagine;

    std::vector<unsigned char> x(rg.get_std_size(100, 1000));

    memory::block b(x.data(), span_size(x.size()));

    span<T> s = eagine::memory::accommodate<T>(b);

    test.check_equal(s.size(), span_size(x.size() / sizeof(T)), "size is ok");
}
//------------------------------------------------------------------------------
void block_7(auto& s) {
    eagitest::case_ test{s, 7, "7"};
    for(unsigned i = 0; i < test.repeats(100); ++i) {
        memory_block_7<char>(test);
        memory_block_7<short>(test);
        memory_block_7<float>(test);
        memory_block_7<int>(test);
        memory_block_7<long>(test);
        memory_block_7<double>(test);
    }
}
//------------------------------------------------------------------------------
template <typename T, typename Case>
void memory_block_8(Case& test) {
    auto& rg{test.random()};
    using namespace eagine;

    std::vector<T> x(rg.get_std_size(100, 1000));

    memory::block b = as_bytes(eagine::memory::cover(x));

    test.check_equal(b.size(), span_size(x.size() * sizeof(T)), "size is ok");
}
//------------------------------------------------------------------------------
void block_8(auto& s) {
    eagitest::case_ test{s, 8, "8"};
    for(unsigned i = 0; i < test.repeats(100); ++i) {
        memory_block_8<char>(test);
        memory_block_8<short>(test);
        memory_block_8<float>(test);
        memory_block_8<int>(test);
        memory_block_8<long>(test);
        memory_block_8<double>(test);
    }
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "block", 8};
    test.once(block_1);
    test.once(block_2);
    test.once(block_3);
    test.once(block_4);
    test.once(block_5);
    test.once(block_6);
    test.once(block_7);
    test.once(block_8);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
