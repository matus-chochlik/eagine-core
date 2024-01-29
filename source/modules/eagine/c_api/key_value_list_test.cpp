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
struct mock_kv_list_traits {
    using key_type = char;
    using conv_type = int;
    using value_type = int;

    static constexpr auto terminator() -> int {
        return -1;
    }
};

using test_key_val_list = eagine::c_api::key_value_list<mock_kv_list_traits>;

using test_kv = eagine::c_api::key_value_list_element<mock_kv_list_traits>;
//------------------------------------------------------------------------------
void key_value_list_1(auto& s) {
    eagitest::case_ test{s, 1, "1"};

    test_key_val_list l0;
    test.check_equal(l0.size(), 1, "0");

    test_key_val_list l2 = l0.append(test_kv('0', 1));
    test.check_equal(l2.size(), 3, "A");
    test.check_equal(l2.data()[0], 0 + '0', "B");
    test.check_equal(l2.data()[1], 1, "C");
    test.check_equal(l2.data()[2], -1, "D");

    test_key_val_list l4 = l2.append(test_kv('2', 3));
    test.check_equal(l4.size(), 5, "E");
    test.check_equal(l4.data()[0], 0 + '0', "F");
    test.check_equal(l4.data()[1], 1, "G");
    test.check_equal(l4.data()[2], 2 + '0', "H");
    test.check_equal(l4.data()[3], 3, "I");
    test.check_equal(l4.data()[4], -1, "J");

    test_key_val_list l6 = l4.append(test_kv('4', 5));
    test.check_equal(l6.size(), 7, "K");
    test.check_equal(l6.data()[0], 0 + '0', "L");
    test.check_equal(l6.data()[1], 1, "M");
    test.check_equal(l6.data()[2], 2 + '0', "N");
    test.check_equal(l6.data()[3], 3, "O");
    test.check_equal(l6.data()[4], 4 + '0', "P");
    test.check_equal(l6.data()[5], 5, "Q");
    test.check_equal(l6.data()[6], -1, "R");

    test_key_val_list l8 = l6.append(test_kv('6', 7));
    test.check_equal(l8.size(), 9, "S");
    test.check_equal(l8.data()[0], 0 + '0', "T");
    test.check_equal(l8.data()[1], 1, "U");
    test.check_equal(l8.data()[2], 2 + '0', "V");
    test.check_equal(l8.data()[3], 3, "W");
    test.check_equal(l8.data()[4], 4 + '0', "X");
    test.check_equal(l8.data()[5], 5, "Y");
    test.check_equal(l8.data()[6], 6 + '0', "Z");
    test.check_equal(l8.data()[7], 7, "AA");
    test.check_equal(l8.data()[8], -1, "AB");
}
//------------------------------------------------------------------------------
void key_value_list_2(auto& s) {
    eagitest::case_ test{s, 2, "2"};

    test_key_val_list l0;
    using kv = test_kv;

    test_key_val_list l2 = l0 + kv('A', 1);
    test.check_equal(l2.size(), 3, "A");
    test.check_equal(l2.data()[0], 'A', "B");
    test.check_equal(l2.data()[1], 1, "C");
    test.check_equal(l2.data()[2], -1, "D");

    test_key_val_list l4 = l0 + kv('A', 1) + kv('B', 3);
    test.check_equal(l4.size(), 5, "E");
    test.check_equal(l4.data()[0], 'A', "F");
    test.check_equal(l4.data()[1], 1, "G");
    test.check_equal(l4.data()[2], 'B', "H");
    test.check_equal(l4.data()[3], 3, "I");
    test.check_equal(l4.data()[4], -1, "J");

    test_key_val_list l6 = l0 + kv('A', 1) + kv('B', 3) + kv('C', 5);
    test.check_equal(l6.size(), 7, "K");
    test.check_equal(l6.data()[0], 'A', "L");
    test.check_equal(l6.data()[1], 1, "M");
    test.check_equal(l6.data()[2], 'B', "N");
    test.check_equal(l6.data()[3], 3, "O");
    test.check_equal(l6.data()[4], 'C', "P");
    test.check_equal(l6.data()[5], 5, "Q");
    test.check_equal(l6.data()[6], -1, "R");

    test_key_val_list l8 =
      l0 + kv('A', 1) + kv('B', 3) + kv('C', 5) + kv('D', 7);
    test.check_equal(l8.size(), 9, "S");
    test.check_equal(l8.data()[0], 'A', "T");
    test.check_equal(l8.data()[1], 1, "U");
    test.check_equal(l8.data()[2], 'B', "V");
    test.check_equal(l8.data()[3], 3, "W");
    test.check_equal(l8.data()[4], 'C', "X");
    test.check_equal(l8.data()[5], 5, "Y");
    test.check_equal(l8.data()[6], 'D', "Z");
    test.check_equal(l8.data()[7], 7, "AA");
    test.check_equal(l8.data()[8], -1, "AB");
}
//------------------------------------------------------------------------------
void key_value_list_3(auto& s) {
    eagitest::case_ test{s, 3, "3"};

    using kv = test_kv;

    test_key_val_list l2{kv('A', 1)};
    test.check_equal(l2.size(), 3, "A");
    test.check_equal(l2.data()[0], 0 + 'A', "B");
    test.check_equal(l2.data()[1], 1, "C");
    test.check_equal(l2.data()[2], -1, "D");

    test_key_val_list l4 = kv('A', 1) + kv('C', 3);
    test.check_equal(l4.size(), 5, "E");
    test.check_equal(l4.data()[0], 0 + 'A', "F");
    test.check_equal(l4.data()[1], 1, "G");
    test.check_equal(l4.data()[2], 2 + 'A', "H");
    test.check_equal(l4.data()[3], 3, "I");
    test.check_equal(l4.data()[4], -1, "J");

    test_key_val_list l6 = kv('A', 1) + kv('C', 3) + kv('E', 5);
    test.check_equal(l6.size(), 7, "K");
    test.check_equal(l6.data()[0], 0 + 'A', "L");
    test.check_equal(l6.data()[1], 1, "M");
    test.check_equal(l6.data()[2], 2 + 'A', "N");
    test.check_equal(l6.data()[3], 3, "O");
    test.check_equal(l6.data()[4], 4 + 'A', "P");
    test.check_equal(l6.data()[5], 5, "Q");
    test.check_equal(l6.data()[6], -1, "R");

    test_key_val_list l8 = kv('A', 1) + kv('C', 3) + kv('E', 5) + kv('G', 7);
    test.check_equal(l8.size(), 9, "S");
    test.check_equal(l8.data()[0], 0 + 'A', "T");
    test.check_equal(l8.data()[1], 1, "U");
    test.check_equal(l8.data()[2], 2 + 'A', "V");
    test.check_equal(l8.data()[3], 3, "W");
    test.check_equal(l8.data()[4], 4 + 'A', "X");
    test.check_equal(l8.data()[5], 5, "Y");
    test.check_equal(l8.data()[6], 6 + 'A', "Z");
    test.check_equal(l8.data()[7], 7, "AA");
    test.check_equal(l8.data()[8], -1, "AB");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "key-value list", 3};
    test.once(key_value_list_1);
    test.once(key_value_list_2);
    test.once(key_value_list_3);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
