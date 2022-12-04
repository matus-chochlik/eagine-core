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
    test.check(are_equal(e1.value(), string_view("A")), "value equal");
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
    test.check(are_equal(e2.value(), string_view("ABCDEFGHIJKLMNOP")), "value");
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

    std::string ss3(128, 'x');
    test.check(are_equal(e3.value(), string_view(ss3)), "value");
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
    string_list::for_each_elem(ss1, [&](string_list::element e, bool) {
        ts.append(e.value().data(), std_size(e.value().size()));
    });
    test.check_equal(ts, "AbCDeFgHiJKlMnO", "value");
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
        ts.append(e.value().data(), std_size(e.value().size()));
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
      ss1, [&ts](string_view e) { ts.append(e.data(), std_size(e.size())); });
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
      ss1, [&](string_view e) { ts.append(e.data(), std_size(e.size())); });
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
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "list", 11};
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
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
