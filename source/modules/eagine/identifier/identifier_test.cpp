/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.types;
import eagine.core.identifier;
//------------------------------------------------------------------------------
void identifier_default_construct(auto& s) {
    eagitest::case_ test{s, 1, "default construct"};
    eagine::identifier i;
    test.constructed(i, "i");

    test.check(!i, "is false");
    test.check_equal(i.size(), 0U, "size is zero");
    test.check_equal(i.name().size(), 0U, "name size is zero");
    test.check_equal(i.name().str().size(), 0U, "name string size is zero");
}
//------------------------------------------------------------------------------
void identifier_from_string(auto& s) {
    eagitest::case_ test{s, 2, "construct from string"};
    auto& rg{test.random()};
    using eagine::span_size;
    using eagine::std_size;

    const auto l = eagine::identifier::max_size();
    char name[l + 1];

    for(int k = 0; k < 1000; ++k) {
        const auto n = rg.get_span_size(0, l - 1);
        for(auto i = span_size(0); i < n; ++i) {
            name[i] = rg.get_char_from(eagine::identifier::encoding::chars());
        }
        name[n] = '\0';

        const eagine::identifier id{name};
        test.check_equal(id.size(), n, "equal size");

        const std::string s1(name, std_size(n));
        const std::string s2 = id.str();

        test.check_equal(s1, s2, "equal name");
    }
}
//------------------------------------------------------------------------------
void identifier_name_cmp(auto& s) {
    eagitest::case_ test{s, 3, "name string compare"};
    using eagine::identifier;

    test.check_equal(identifier{"A"}.str(), "A", "1");
    test.check_equal(identifier{"BC"}.str(), "BC", "2");
    test.check_equal(identifier{"CDE"}.str(), "CDE", "3");
    test.check_equal(identifier{"DEFG"}.str(), "DEFG", "4");
    test.check_equal(identifier{"EFGHI"}.str(), "EFGHI", "5");
    test.check_equal(identifier{"FGHIJK"}.str(), "FGHIJK", "6");
    test.check_equal(identifier{"GHIJKLM"}.str(), "GHIJKLM", "7");
    test.check_equal(identifier{"HIJKLMNO"}.str(), "HIJKLMNO", "8");
    test.check_equal(identifier{"IJKLMNOPQ"}.str(), "IJKLMNOPQ", "9");
    test.check_equal(identifier{"JKLMNOPQRS"}.str(), "JKLMNOPQRS", "10");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "identifier", 3};
    test.once(identifier_default_construct);
    test.once(identifier_from_string);
    test.once(identifier_name_cmp);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
