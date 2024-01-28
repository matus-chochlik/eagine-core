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
import eagine.core.string;
//------------------------------------------------------------------------------
void str_var_subst_1(auto& s) {
    eagitest::case_ test{s, 1, "subst 1"};

    std::map<std::string, std::string, eagine::str_view_less> empty;

    test.check_equal(
      eagine::substitute_variables(std::string(), empty), std::string(), "A");

    test.check_equal(
      eagine::substitute_variables(std::string("abcdefgh{ijk}lmn"), empty),
      std::string("abcdefgh{ijk}lmn"),
      "B");

    test.check_equal(
      eagine::substitute_variables(std::string("abcdefgh${ijk}lmn"), empty),
      std::string("abcdefghlmn"),
      "C");

    test.check_equal(
      eagine::substitute_variables(std::string("abcdefgh${}lmn"), empty),
      std::string("abcdefgh${}lmn"),
      "D");
}
//------------------------------------------------------------------------------
void str_var_subst_2(auto& s) {
    eagitest::case_ test{s, 2, "subst 2"};

    std::map<std::string, std::string, eagine::str_view_less> dict;
    dict["A"] = "1";
    dict["B"] = "2";
    dict["C"] = "3";
    dict["D"] = "4";

    test.check_equal(
      eagine::substitute_variables(std::string(), dict), std::string(), "A");

    test.check_equal(
      eagine::substitute_variables(std::string("a{bc{defg}h{ijk}l}mn"), dict),
      std::string("a{bc{defg}h{ijk}l}mn"),
      "B");

    test.check_equal(
      eagine::substitute_variables(std::string("bla+ble$"), dict),
      std::string("bla+ble$"),
      "C");

    test.check_equal(
      eagine::substitute_variables(std::string("$bla+ble"), dict),
      std::string("$bla+ble"),
      "D");

    test.check_equal(
      eagine::substitute_variables(std::string("$bla$ble$"), dict),
      std::string("$bla$ble$"),
      "E");

    test.check_equal(
      eagine::substitute_variables(std::string("{${A}+${B}}"), dict),
      std::string("{1+2}"),
      "F");

    test.check_equal(
      eagine::substitute_variables(std::string("{${E}+${F}}"), dict),
      std::string("{+}"),
      "G");

    eagine::variable_substitution_options opts;
    opts.keep_untranslated = false;
    test.check_equal(
      eagine::substitute_variables(std::string("{${E}+${F}}"), dict, opts),
      std::string("{+}"),
      "H");

    opts.keep_untranslated = true;
    test.check_equal(
      eagine::substitute_variables(std::string("{${E}+${F}}"), dict, opts),
      std::string("{${E}+${F}}"),
      "I");

    test.check_equal(
      eagine::substitute_variables(std::string("${A}_${B}-${C}+${D}"), dict),
      std::string("1_2-3+4"),
      "J");

    test.check_equal(
      eagine::substitute_variables(std::string("${A}_${E}_${D}"), dict, opts),
      std::string("1_${E}_4"),
      "K");

    opts.keep_untranslated = false;
    test.check_equal(
      eagine::substitute_variables(std::string("${A}_${E}_${D}"), dict, opts),
      std::string("1__4"),
      "L");
}
//------------------------------------------------------------------------------
void str_var_subst_3(auto& s) {
    eagitest::case_ test{s, 3, "subst 3"};

    std::map<std::string, std::string, eagine::str_view_less> dict;
    dict["A"] = "1";
    dict["B"] = "2";
    dict["1+2"] = "C";
    dict["C"] = "3";
    dict["2+2"] = "D";
    dict["D"] = "4";
    dict["4-3"] = "A";

    test.check_equal(
      eagine::substitute_variables(std::string(), dict), std::string(), "A");

    test.check_equal(
      eagine::substitute_variables(std::string("${${B}+${B}}"), dict),
      std::string("D"),
      "B");

    test.check_equal(
      eagine::substitute_variables(std::string("${${A}+${B}}"), dict),
      std::string("C"),
      "C");

    test.check_equal(
      eagine::substitute_variables(
        std::string("${${${${B}+${B}}}-${${${A}+${B}}}}"), dict),
      std::string("A"),
      "D");

    test.check_equal(
      eagine::substitute_variables(
        std::string("${${${${${B}+${B}}}-${${${A}+${B}}}}}"), dict),
      std::string("1"),
      "E");
}
//------------------------------------------------------------------------------
void str_var_subst_4(auto& s) {
    eagitest::case_ test{s, 4, "subst 4"};

    eagine::string_variable_map vars;
    vars.set("A", "1")
      .set("B", "2")
      .set("C", "3")
      .set("D", "4")
      .set("1+2", "C")
      .set("2+2", "D")
      .set("4-3", "A");

    test.check_equal(
      vars.subst_variables(
        std::string("${${${${${B}+${B}}}-${${${A}+${B}}}}}")),
      std::string("1"),
      "1");
}
//------------------------------------------------------------------------------
void str_var_subst_5(auto& s) {
    eagitest::case_ test{s, 5, "subst 5"};

    std::vector<std::string> ssv{"A", "B", "C", "D", "E"};
    auto ss = eagine::memory::view(ssv);

    test.check_equal(
      eagine::substitute_variables(std::string(), ss), std::string(), "A");

    test.check_equal(
      eagine::substitute_variables(std::string("${1}${2}${3}${4}${5}"), ss),
      std::string("ABCDE"),
      "B");

    test.check_equal(
      eagine::substitute_variables(std::string("${1}${3}${5}${7}"), ss),
      std::string("ACE"),
      "C");

    eagine::variable_substitution_options opts;
    opts.keep_untranslated = true;
    test.check_equal(
      eagine::substitute_variables(std::string("${1}${3}${5}${7}"), ss, opts),
      std::string("ACE${7}"),
      "D");

    test.check_equal(
      eagine::substitute_variables(std::string("${1}${1}${1}${1}${1}"), ss),
      std::string("AAAAA"),
      "E");

    test.check_equal(
      eagine::substitute_variables(std::string("${7}${11}${13}"), ss),
      std::string(),
      "F");

    test.check_equal(
      eagine::substitute_variables(std::string("${7}${11}${13}"), ss, opts),
      std::string("${7}${11}${13}"),
      "G");

    test.check_equal(
      eagine::substitute_variables(std::string("${4}${2}${1}${3}${5}"), ss),
      std::string("DBACE"),
      "H");

    test.check_equal(
      eagine::substitute_variables(std::string("<${0}>"), ss),
      std::string("<>"),
      "I");

    test.check_equal(
      eagine::substitute_variables(std::string("-${6}-"), ss),
      std::string("--"),
      "I");

    test.check_equal(
      eagine::substitute_variables(std::string("${2}-${5}-${2}-${5}"), ss),
      std::string("B-E-B-E"),
      "J");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "format", 5};
    test.once(str_var_subst_1);
    test.once(str_var_subst_2);
    test.once(str_var_subst_3);
    test.once(str_var_subst_4);
    test.once(str_var_subst_5);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
