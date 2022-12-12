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
void list_empty(auto& s) {
    eagitest::case_ test{s, 1, "empty"};
    using namespace eagine;

    const char s0[2] = {0x00, 0x00};
    string_list::element e0(s0, 2);

    test.check_equal(e0.size(), 2, "size");
    test.check_equal(e0.header_size(), 1, "header size");
    test.check_equal(e0.footer_size(), 1, "footer size");
    test.check_equal(e0.value_size(), 0, "value size");
    test.check_equal(string_list::element_count(s0), 0, "count 0");
}
//------------------------------------------------------------------------------
void list_element_1(auto& s) {
    eagitest::case_ test{s, 2, "element 1"};
    using namespace eagine;

    const char s1[3] = {0x01, 'A', 0x01};
    string_list::element e1(s1, 3);

    test.check_equal(e1.size(), 3, "size");
    test.check_equal(e1.header_size(), 1, "header size");
    test.check_equal(e1.footer_size(), 1, "footer size");
    test.check_equal(e1.value_size(), 1, "value size");
    test.check_equal(string_list::element_count(s1), 1, "count 1");
    test.check_equal(e1.value(), string_view("A"), "value equal");
    test.check_equal(
      string_list::front_value(s1), string_view("A"), "front equal");
    test.check_equal(
      string_list::back_value(s1), string_view("A"), "back equal");
}
//------------------------------------------------------------------------------
void list_element_2(auto& s) {
    eagitest::case_ test{s, 3, "element 2"};
    using namespace eagine;

    const char s2[18] = {
      0x10,
      'A',
      'B',
      'C',
      'D',
      'E',
      'F',
      'G',
      'H',
      'I',
      'J',
      'K',
      'L',
      'M',
      'N',
      'O',
      'P',
      0x10};
    string_list::element e2(s2, 18);

    test.check_equal(e2.size(), 18, "size");
    test.check_equal(e2.header_size(), 1, "header size");
    test.check_equal(e2.footer_size(), 1, "footer size");
    test.check_equal(e2.value_size(), 16, "value size");
    test.check_equal(string_list::element_count(s2), 1, "count 1");
    test.check_equal(e2.value(), string_view("ABCDEFGHIJKLMNOP"), "value");
    test.check_equal(
      string_list::front_value(s2),
      string_view("ABCDEFGHIJKLMNOP"),
      "front value");
    test.check_equal(
      string_list::back_value(s2),
      string_view("ABCDEFGHIJKLMNOP"),
      "back value");
}
//------------------------------------------------------------------------------
void list_element_3(auto& s) {
    eagitest::case_ test{s, 4, "element 3"};
    using namespace eagine;

    const char s3[132] = {
      char(0xC2), char(0x80), 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
      'x',        'x',        'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
      'x',        'x',        'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
      'x',        'x',        'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
      'x',        'x',        'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
      'x',        'x',        'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
      'x',        'x',        'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
      'x',        'x',        'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
      'x',        'x',        'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
      'x',        'x',        'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
      'x',        'x',        'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
      'x',        'x',        'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
      'x',        'x',        'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
      char(0xC2), char(0x80)};
    string_list::element e3(s3, 132);

    test.check_equal(e3.size(), 132, "size");
    test.check_equal(e3.header_size(), 2, "header size");
    test.check_equal(e3.footer_size(), 2, "footer size");
    test.check_equal(e3.value_size(), 128, "value size");
    test.check_equal(string_list::element_count(s3), 1, "count 1");

    std::string ss3(128, 'x');
    test.check_equal(e3.value(), string_view(ss3), "value");
    test.check_equal(
      string_list::front_value(s3), string_view(ss3), "front value");
    test.check_equal(
      string_list::back_value(s3), string_view(ss3), "back value");
}
//------------------------------------------------------------------------------
void list_for_each_element_1(auto& s) {
    eagitest::case_ test{s, 5, "for each element 1"};
    using namespace eagine;

    const char s1[25] = {0x01, 'A', 0x01, 0x02, 'b', 'C', 0x02, 0x03, 'D',
                         'e',  'F', 0x03, 0x04, 'g', 'H', 'i',  'J',  0x04,
                         0x05, 'K', 'l',  'M',  'n', 'O', 0x05};
    string_view ss1(s1, 25);

    int i = 1;
    string_list::for_each_elem(ss1, [&](string_list::element e, bool first) {
        test.check_equal(first, i == 1, "first");
        test.check_equal(e.value_size(), i++, "value size");
    });

    std::string ts;
    test.check_equal(string_list::element_count(ss1), 5, "count 5");
    string_list::for_each_elem(ss1, [&](string_list::element e, bool) {
        ts.append(e.value().data(), e.value().std_size());
    });
    test.check_equal(ts, "AbCDeFgHiJKlMnO", "value");
    test.check_equal(
      string_list::front_value(ss1), string_view("A"), "front value");
    test.check_equal(
      string_list::back_value(ss1), string_view("KlMnO"), "back value");
}
//------------------------------------------------------------------------------
void list_for_each_element_2(auto& s) {
    eagitest::case_ test{s, 6, "for each element 2"};
    using namespace eagine;

    const char s1[25] = {0x01, 'A', 0x01, 0x02, 'b', 'C', 0x02, 0x03, 'D',
                         'e',  'F', 0x03, 0x04, 'g', 'H', 'i',  'J',  0x04,
                         0x05, 'K', 'l',  'M',  'n', 'O', 0x05};
    string_view ss1(s1, 25);

    int i = 5;
    string_list::rev_for_each_elem(
      ss1, [&](string_list::element e, bool first) {
          test.check_equal(first, i == 5, "first");
          test.check_equal(e.value_size(), i--, "value size");
      });

    std::string ts;
    string_list::rev_for_each_elem(ss1, [&](string_list::element e, bool) {
        ts.append(e.value().data(), e.value().std_size());
    });
    test.check_equal(ts, "KlMnOgHiJDeFbCA", "value");
}
//------------------------------------------------------------------------------
void list_for_each_1(auto& s) {
    eagitest::case_ test{s, 7, "for each 1"};
    using namespace eagine;

    const char s1[25] = {0x01, 'a', 0x01, 0x02, 'B', 'c', 0x02, 0x03, 'd',
                         'E',  'f', 0x03, 0x04, 'G', 'h', 'I',  'j',  0x04,
                         0x05, 'k', 'L',  'm',  'N', 'o', 0x05};
    string_view ss1(s1, 25);

    int i = 1;
    string_list::for_each(
      ss1, [&](string_view e) { test.check_equal(e.size(), i++, "size"); });

    std::string ts;
    string_list::for_each(
      ss1, [&ts](string_view e) { ts.append(e.data(), e.std_size()); });
    test.check_equal(ts, "aBcdEfGhIjkLmNo", "value");
}
//------------------------------------------------------------------------------
void list_for_each_2(auto& s) {
    eagitest::case_ test{s, 8, "for each 2"};
    using namespace eagine;

    const char s1[25] = {0x01, 'a', 0x01, 0x02, 'B', 'c', 0x02, 0x03, 'd',
                         'E',  'f', 0x03, 0x04, 'G', 'h', 'I',  'j',  0x04,
                         0x05, 'k', 'L',  'm',  'N', 'o', 0x05};
    string_view ss1(s1, 25);

    int i = 5;
    string_list::rev_for_each(
      ss1, [&](string_view e) { test.check_equal(e.size(), i--, "size"); });

    std::string ts;
    string_list::rev_for_each(
      ss1, [&](string_view e) { ts.append(e.data(), e.std_size()); });
    test.check_equal(ts, "kLmNoGhIjdEfBca", "value");
}
//------------------------------------------------------------------------------
void list_join_1(auto& s) {
    eagitest::case_ test{s, 9, "join 1"};
    using namespace eagine;

    const char s1[25] = {0x01, 'A', 0x01, 0x02, 'B', 'C', 0x02, 0x03, 'D',
                         'E',  'F', 0x03, 0x04, 'G', 'H', 'I',  'J',  0x04,
                         0x05, 'K', 'L',  'M',  'N', 'O', 0x05};
    string_view ss1(s1, 25);
    string_view sep("");

    test.check_equal(
      string_list::join(ss1, sep, true), "ABCDEFGHIJKLMNO", "joined");
}
//------------------------------------------------------------------------------
void list_join_2(auto& s) {
    eagitest::case_ test{s, 10, "join 2"};
    using namespace eagine;

    const char s1[25] = {0x01, 'A', 0x01, 0x02, 'B', 'C', 0x02, 0x03, 'D',
                         'E',  'F', 0x03, 0x04, 'G', 'H', 'I',  'J',  0x04,
                         0x05, 'K', 'L',  'M',  'N', 'O', 0x05};
    string_view ss1(s1, 25);
    string_view sep("/");

    test.check_equal(
      string_list::join(ss1, sep, true), "A/BC/DEF/GHIJ/KLMNO/", "joined");
}
//------------------------------------------------------------------------------
void list_join_3(auto& s) {
    eagitest::case_ test{s, 11, "join 3"};
    using namespace eagine;

    const char s1[25] = {0x01, 'A', 0x01, 0x02, 'B', 'C', 0x02, 0x03, 'D',
                         'E',  'F', 0x03, 0x04, 'G', 'H', 'I',  'J',  0x04,
                         0x05, 'K', 'L',  'M',  'N', 'O', 0x05};
    string_view ss1(s1, 25);
    string_view sep("...");

    test.check_equal(
      string_list::join(ss1, sep, false),
      "A...BC...DEF...GHIJ...KLMNO",
      "joined");
}
//------------------------------------------------------------------------------
void list_push_back_1(auto& s) {
    eagitest::case_ test{s, 12, "push back 1"};
    using namespace eagine;

    std::string sp;
    test.check_equal(string_list::element_count(sp), 0, "size 0");

    string_list::push_back(sp, "ABCD");
    test.check_equal(string_list::element_count(sp), 1, "size 1");
    test.check_equal(
      string_list::front_value(sp), string_view("ABCD"), "front value 1");
    test.check_equal(
      string_list::back_value(sp), string_view("ABCD"), "back value 1");

    string_list::push_back(sp, "EFGHI");
    test.check_equal(string_list::element_count(sp), 2, "size 2");
    test.check_equal(
      string_list::front_value(sp), string_view("ABCD"), "front value 2");
    test.check_equal(
      string_list::back_value(sp), string_view("EFGHI"), "back value 2");

    string_list::push_back(sp, "JKLMNO");
    test.check_equal(string_list::element_count(sp), 3, "size 3");
    test.check_equal(
      string_list::front_value(sp), string_view("ABCD"), "front value 3");
    test.check_equal(
      string_list::back_value(sp), string_view("JKLMNO"), "back value 3");

    string_list::push_back(sp, "PQRSTUV");
    test.check_equal(string_list::element_count(sp), 4, "size 4");
    test.check_equal(
      string_list::front_value(sp), string_view("ABCD"), "front value 4");
    test.check_equal(
      string_list::back_value(sp), string_view("PQRSTUV"), "back value 4");

    string_list::push_back(sp, "WXYZ");
    test.check_equal(string_list::element_count(sp), 5, "size 5");
    test.check_equal(
      string_list::front_value(sp), string_view("ABCD"), "front value 5");
    test.check_equal(
      string_list::back_value(sp), string_view("WXYZ"), "back value 5");
}
//------------------------------------------------------------------------------
void list_push_back_2(auto& s) {
    eagitest::case_ test{s, 13, "push back 2"};
    using namespace eagine;

    const std::string ex(128, 'x');
    const std::string ey(256, 'y');
    const std::string ez(999, 'z');

    std::string sp;
    test.check_equal(string_list::element_count(sp), 0, "size 0");

    string_list::push_back(sp, ex);
    test.check_equal(string_list::element_count(sp), 1, "size 1");
    test.check_equal(string_list::front_value(sp), ex, "front value 1");
    test.check_equal(string_list::back_value(sp), ex, "back value 1");

    string_list::push_back(sp, ey);
    test.check_equal(string_list::element_count(sp), 2, "size 2");
    test.check_equal(string_list::front_value(sp), ex, "front value 2");
    test.check_equal(string_list::back_value(sp), ey, "back value 2");

    string_list::push_back(sp, ez);
    test.check_equal(string_list::element_count(sp), 3, "size 3");
    test.check_equal(string_list::front_value(sp), ex, "front value 3");
    test.check_equal(string_list::back_value(sp), ez, "back value 3");
}
//------------------------------------------------------------------------------
void list_push_pop_1(auto& s) {
    eagitest::case_ test{s, 14, "push pop 1"};
    auto& rg{test.random()};
    using namespace eagine;

    std::string sp;
    test.check_equal(string_list::element_count(sp), 0, "size 0");

    for(unsigned i = 0; i < test.repeats(100); ++i) {
        const auto e{rg.get_string(0, rg.one_of(7) ? 0 : 8192)};

        string_list::push_back(sp, e);
        test.check_equal(string_list::element_count(sp), 1, "size 1");
        test.check_equal(string_list::front_value(sp), e, "front value");
        test.check_equal(string_list::back_value(sp), e, "back value");

        test.ensure(string_list::element_count(sp) > 0, "not empty");
        string_list::pop_back(sp);
    }
    test.check_equal(string_list::element_count(sp), 0, "size 0");
}
//------------------------------------------------------------------------------
void list_push_pop_2(auto& s) {
    eagitest::case_ test{s, 15, "push pop 2"};
    auto& rg{test.random()};
    using namespace eagine;

    std::string sp;
    const auto f{rg.get_string(0, rg.one_of(7) ? 0 : 1024)};
    string_list::push_back(sp, f);
    test.check_equal(string_list::element_count(sp), 1, "size 1");

    for(unsigned i = 0; i < test.repeats(100); ++i) {
        const auto e{rg.get_string(0, rg.one_of(7) ? 0 : 4096)};

        string_list::push_back(sp, e);
        test.check_equal(string_list::element_count(sp), 2, "size 2");
        test.check_equal(string_list::front_value(sp), f, "front value");
        test.check_equal(string_list::back_value(sp), e, "back value");

        test.ensure(string_list::element_count(sp) > 1, "not empty");
        string_list::pop_back(sp);
    }
    test.check_equal(string_list::element_count(sp), 1, "size 1");
}
//------------------------------------------------------------------------------
void list_push_pop_3(auto& s) {
    eagitest::case_ test{s, 16, "push pop 3"};
    auto& rg{test.random()};
    using namespace eagine;

    std::string sp;
    const auto f{rg.get_string(0, rg.one_of(7) ? 0 : 2048)};
    string_list::push_back(sp, f);
    test.check_equal(string_list::element_count(sp), 1, "size 1");

    span_size_t count{1};
    for(unsigned i = 0; i < test.repeats(100); ++i) {
        const auto e{rg.get_string(0, rg.one_of(7) ? 0 : 2048)};

        string_list::push_back(sp, e);
        ++count;
        test.check_equal(string_list::element_count(sp), count, "size n");
        test.check_equal(string_list::front_value(sp), f, "front value");
        test.check_equal(string_list::back_value(sp), e, "back value");

        if(rg.one_of(5)) {
            if(string_list::element_count(sp) > 0) {
                string_list::pop_back(sp);
                --count;
            }
        }
    }
    while(count > 0) {
        test.ensure(string_list::element_count(sp) == count, "count ok");
        string_list::pop_back(sp);
        --count;
    }
    test.check_equal(string_list::element_count(sp), 0, "size 0");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "list", 16};
    test.once(list_empty);
    test.once(list_element_1);
    test.once(list_element_2);
    test.once(list_element_3);
    test.once(list_for_each_element_1);
    test.once(list_for_each_element_2);
    test.once(list_for_each_1);
    test.once(list_for_each_2);
    test.once(list_join_1);
    test.once(list_join_2);
    test.once(list_join_3);
    test.once(list_push_back_1);
    test.once(list_push_back_2);
    test.once(list_push_pop_1);
    test.once(list_push_pop_2);
    test.once(list_push_pop_3);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
