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
import eagine.core.c_api;
//------------------------------------------------------------------------------
void c_api_buffer_size_1(auto& s) {
    eagitest::case_ test{s, 1, "size 1"};

    eagine::c_api::buffer_size<int> s0;
    test.check_equal(int(s0), 0, "1");
    test.check_equal(long(s0), 0L, "2");
    test.check_equal(std::size_t(s0), 0U, "3");

    eagine::c_api::buffer_size<int> s1(12345);
    test.check_equal(int(s1), 12345, "4");
    test.check_equal(long(s1), 12345L, "5");
    test.check_equal(std::size_t(s1), 12345U, "6");
}
//------------------------------------------------------------------------------
void c_api_buffer_size_2(auto& s) {
    eagitest::case_ test{s, 2, "size 2"};
    eagitest::track trck{test, 0, 5};
    auto& rg{test.random()};

    const auto get_n{[&] {
        return rg.get_span_size(0, 1000000);
    }};

    eagine::span_size_t n{};
    {
        n = get_n();
        eagine::c_api::buffer_size<int> st(std::type_identity<char>(), n);
        test.check_equal(st.get(), int(eagine::span_size_of<char>() * n), "1");
        test.check_equal(int(st), int(eagine::span_size_of<char>() * n), "2");
        trck.checkpoint(1);
    }
    {
        n = get_n();
        eagine::c_api::buffer_size<int> st(std::type_identity<short>(), n);
        test.check_equal(st.get(), int(eagine::span_size_of<short>() * n), "3");
        test.check_equal(int(st), int(eagine::span_size_of<short>() * n), "4");
        trck.checkpoint(2);
    }
    {
        n = get_n();
        eagine::c_api::buffer_size<int> st(std::type_identity<unsigned>(), n);
        test.check_equal(
          st.get(), int(eagine::span_size_of<unsigned>() * n), "5");
        test.check_equal(
          int(st), int(eagine::span_size_of<unsigned>() * n), "6");
        trck.checkpoint(3);
    }
    {
        n = get_n();
        eagine::c_api::buffer_size<int> st(std::type_identity<long>(), n);
        test.check_equal(st.get(), int(eagine::span_size_of<long>() * n), "7");
        test.check_equal(int(st), int(eagine::span_size_of<long>() * n), "8");
        trck.checkpoint(4);
    }
    {
        n = get_n();
        eagine::c_api::buffer_size<int> st(std::type_identity<float>(), n);
        test.check_equal(st.get(), int(eagine::span_size_of<float>() * n), "9");
        test.check_equal(int(st), int(eagine::span_size_of<float>() * n), "10");
        trck.checkpoint(5);
    }
}
//------------------------------------------------------------------------------
void c_api_buffer_size_3(auto& s) {
    eagitest::case_ test{s, 3, "size 3"};
    eagitest::track trck{test, 0, 5};
    auto& rg{test.random()};

    const auto get_n{[&] {
        return rg.get_span_size(0, 1000000);
    }};

    eagine::span_size_t n{};
    {
        n = get_n();
        eagine::c_api::buffer_size<unsigned> st(std::type_identity<char>(), n);
        test.check_equal(
          unsigned(st), unsigned(eagine::span_size_of<char>() * n), "1");
        trck.checkpoint(1);
    }
    {
        n = get_n();
        eagine::c_api::buffer_size<unsigned> st(std::type_identity<short>(), n);
        test.check_equal(
          unsigned(st), unsigned(eagine::span_size_of<short>() * n), "2");
        trck.checkpoint(2);
    }
    {
        n = get_n();
        eagine::c_api::buffer_size<unsigned> st(std::type_identity<int>(), n);
        test.check_equal(
          unsigned(st), unsigned(eagine::span_size_of<int>() * n), "3");
        trck.checkpoint(3);
    }
    {
        n = get_n();
        eagine::c_api::buffer_size<unsigned> st(std::type_identity<long>(), n);
        test.check_equal(
          unsigned(st), unsigned(eagine::span_size_of<long>() * n), "4");
        trck.checkpoint(4);
    }
    {
        n = get_n();
        eagine::c_api::buffer_size<unsigned> st(std::type_identity<float>(), n);
        test.check_equal(
          unsigned(st), unsigned(eagine::span_size_of<float>() * n), "5");
        trck.checkpoint(5);
    }
}
//------------------------------------------------------------------------------
void c_api_buffer_size_4(auto& s) {
    eagitest::case_ test{s, 4, "size 4"};
    eagitest::track trck{test, 0, 3};

    {
        const char bla[4] = {'b', 'l', 'a', '\0'};
        eagine::c_api::buffer_size<int> st{eagine::view(bla)};
        test.check_equal(st.get(), int(eagine::span_size_of<char>() * 4), "1");
        test.check_equal(int(st), int(eagine::span_size_of<char>() * 4), "2");
        trck.checkpoint(1);
    }
    {
        const unsigned short csv[7] = {1, 2, 3, 4, 5, 6, 7};
        eagine::c_api::buffer_size<long> st{eagine::view(csv)};
        test.check_equal(
          st.get(), long(eagine::span_size_of<short>() * 7), "3");
        test.check_equal(
          long(st), long(eagine::span_size_of<short>() * 7), "4");
        trck.checkpoint(2);
    }
    {
        const int civ[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        eagine::c_api::buffer_size<unsigned> st{eagine::view(civ)};
        test.check_equal(
          st.get(), unsigned(eagine::span_size_of<int>() * 10), "5");
        test.check_equal(
          unsigned(st), unsigned(eagine::span_size_of<int>() * 10), "6");
        trck.checkpoint(3);
    }
}
//------------------------------------------------------------------------------
void c_api_buffer_data_1(auto& s) {
    eagitest::case_ test{s, 5, "data 1"};

    eagine::c_api::buffer_data_spec<int> bd;
    test.check(bd.empty(), "empty");
    test.check_equal(int(bd.size()), 0, "size int");
    test.check_equal(long(bd.size()), 0L, "size long");
    test.check_equal(std::size_t(bd.size()), 0U, "size");
}
//------------------------------------------------------------------------------
void c_api_buffer_data_2(auto& s) {
    eagitest::case_ test{s, 6, "data 2"};
    eagitest::track trck{test, 0, 2};

    {
        const long clv[8] = {0, 1, 2, 3, 4, 5, 6, 7};

        eagine::c_api::buffer_data_spec<int> bd(clv);
        test.check(!bd.empty(), "not empty int");
        test.check(bd.data() != nullptr, "data not null int");
        test.check(bd.data() == static_cast<const void*>(clv), "data ok int");
        test.check(int(bd.size()) == int(sizeof(long) * 8), "size ok int");
        test.check(
          unsigned(bd.size()) == unsigned(sizeof(long) * 8), "size ok int");
        trck.checkpoint(1);
    }

    {
        float fv[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

        eagine::c_api::buffer_data_spec<long> bd(fv);
        test.check(!bd.empty(), "not empty long");
        test.check(bd.data() != nullptr, "data not null long");
        test.check(bd.data() == static_cast<const void*>(fv), "data ok long");
        test.check(long(bd.size()) == long(sizeof(float) * 10), "size ok long");
        trck.checkpoint(2);
    }
}
//------------------------------------------------------------------------------
void c_api_buffer_data_3(auto& s) {
    eagitest::case_ test{s, 7, "data 3"};
    eagitest::track trck{test, 0, 3};

    {
        eagine::c_api::buffer_data_spec<int> bd;
        eagine::memory::span<const eagine::byte> v = bd.view();

        test.check(v.begin() == v.end(), "begin == end int");
        trck.checkpoint(1);
    }

    {
        const short csv[8] = {0, 1, 2, 3, 4, 5, 6, 7};

        eagine::c_api::buffer_data_spec<short> bd(csv);
        eagine::memory::span<const eagine::byte> v = bd.view();

        test.check(v.begin() != v.end(), "begin == end short");
        trck.checkpoint(2);
    }

    {
        double fv[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

        eagine::c_api::buffer_data_spec<unsigned> bd(fv);
        eagine::memory::span<const eagine::byte> v = bd.view();

        test.check(v.begin() != v.end(), "begin == end unsigned");
        trck.checkpoint(3);
    }
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "buffer", 7};
    test.once(c_api_buffer_size_1);
    test.once(c_api_buffer_size_2);
    test.once(c_api_buffer_size_3);
    test.once(c_api_buffer_size_4);
    test.once(c_api_buffer_data_1);
    test.once(c_api_buffer_data_2);
    test.once(c_api_buffer_data_3);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
