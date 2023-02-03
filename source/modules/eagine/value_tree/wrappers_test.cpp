/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.value_tree;
//------------------------------------------------------------------------------
void attribute_default_construct(auto& s) {
    eagitest::case_ test{s, 1, "attribute default construct"};

    eagine::valtree::attribute a;

    test.check(not a, "is false");
}
//------------------------------------------------------------------------------
void compound_default_construct(auto& s) {
    eagitest::case_ test{s, 2, "compound default construct"};

    eagine::valtree::attribute a;
    eagine::valtree::compound c;

    test.check(not c, "is false");
    test.check(not bool(c), "is not true");
    test.check_equal(c.nested_count(a), 0, "nested count");
    test.check(not c.has_nested(a), "has not nested");
    test.check(not c.nested(a, 0), "not nested 0");
    test.check(not c.nested(a, 1), "not nested 1");
}
//------------------------------------------------------------------------------
void compound_attribute_default_construct(auto& s) {
    eagitest::case_ test{s, 3, "compound attribute default construct"};

    eagine::valtree::compound_attribute ca;

    test.check(not ca, "is false");
    test.check(not bool(ca), "is not true");
    test.check_equal(ca.nested_count(), 0, "nested count");
    test.check(not ca.has_nested(), "has not nested");
    test.check(not ca.nested(0), "not nested 0");
    test.check(not ca.nested(1), "not nested 1");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "value_tree wrappers", 3};
    test.once(attribute_default_construct);
    test.once(compound_default_construct);
    test.once(compound_attribute_default_construct);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
