/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import std;
import eagine.core.types;
//------------------------------------------------------------------------------
struct inner {
    short s{0};
    bool b{true};

    auto foo() const noexcept -> int {
        return 42;
    }

    auto bar(int i) const noexcept -> int {
        return 2 * i;
    }
};

struct outer {
    inner i{};
};
//------------------------------------------------------------------------------
void placeholder_member_object(auto& s) {
    eagitest::case_ test{s, 1, "member object"};
    using eagine::_1;

    outer o{};
    o.i.s = 123;

    test.check_equal((_1.member(&outer::i))(o).s, o.i.s, "member object 1");
    test.check_equal(
      (_1.member(&outer::i).member(&inner::s))(o), o.i.s, "member object 2");
    test.check_equal(
      (_1.member(&outer::i).member(&inner::b))(o), o.i.b, "member object 3");
}
//------------------------------------------------------------------------------
void placeholder_member_function(auto& s) {
    eagitest::case_ test{s, 2, "member function"};
    using eagine::_1;

    outer o{};

    test.check_equal(
      (_1.member(&outer::i).member(&inner::foo))(o), 42, "member function 1");
    test.check_equal(
      (_1.member(&outer::i).member(&inner::bar, 123))(o),
      246,
      "member function 2");
}
//------------------------------------------------------------------------------
void placeholder_empty(auto& s) {
    eagitest::case_ test{s, 3, "empty"};
    using eagine::_1;

    const std::string empty;
    std::string strng{"eagine"};
    test.check(_1.empty()(empty), "empty");
    test.check(not _1.empty()(strng), "not empty");
}
//------------------------------------------------------------------------------
void placeholder_size(auto& s) {
    eagitest::case_ test{s, 4, "size"};
    using eagine::_1;

    const std::string empty;
    std::string strng{"eagine"};
    test.check_equal(_1.size()(empty), 0U, "size == 0");
    test.check_equal(_1.size()(strng), 6U, "size == 6");
}
//------------------------------------------------------------------------------
void placeholder_has_value(auto& s) {
    eagitest::case_ test{s, 5, "has value"};
    using eagine::_1;

    const std::optional<std::string> empty;
    std::optional<std::string> strng{"eagine"};
    test.check(not _1.has_value()(empty), "has not value");
    test.check(_1.has_value()(strng), "has value");
}
//------------------------------------------------------------------------------
void placeholder_value_or(auto& s) {
    eagitest::case_ test{s, 6, "value/or"};
    using eagine::_1;

    const std::optional<std::string> empty;
    std::optional<std::string> strng{"eagine"};
    test.check_equal(_1.value_or("foo")(empty), "foo", "value or foo");
    test.check_equal(_1.value_or("bar")(strng), "eagine", "value or eagine");
    test.check_equal(_1.value()(strng), "eagine", "value or eagine");
}
//------------------------------------------------------------------------------
void placeholder_and_then(auto& s) {
    eagitest::case_ test{s, 7, "and then"};
    using eagine::_1;

    const std::optional<std::string> empty;
    std::optional<std::string> strng{"eag"};
    const auto func{[](std::string arg) -> std::optional<std::string> {
        return arg + "ine";
    }};
    test.check_equal(
      _1.and_then(func)(empty).value_or("baz"), "baz", "and then empty");
    test.check_equal(
      _1.and_then(func)(strng).value_or("foo"), "eagine", "and then eagine");
    test.check_equal(
      _1.and_then(func).value_or("bar")(empty), "bar", "and then empty");
    test.check_equal(
      _1.and_then(func).value_or("foo")(strng), "eagine", "and then eagine");
    test.check_equal(
      _1.and_then(_1.size().to_optional())(empty).value_or(42),
      42U,
      "and then size 42");
    test.check_equal(
      _1.and_then(_1.size().to_optional())(strng).value_or(1),
      3U,
      "and then size 3");
}
//------------------------------------------------------------------------------
void placeholder_tribool(auto& s) {
    eagitest::case_ test{s, 8, "tribool"};
    using eagine::_1;

    const std::string empty;
    std::string strng{"eagine"};
    std::optional<std::string> opt;

    test.check_equal(bool(_1.return_true()(empty)), true, "true");
    test.check_equal(bool(_1.return_false()(empty)), false, "false");
    test.check_equal(bool(_1.size().return_true()(empty)), true, "size true");
    test.check_equal(
      bool(_1.size().return_false()(strng)), false, "size false");
    test.check_equal(bool(_1.empty().to_tribool()(empty)), true, "empty");
    test.check_equal(bool(_1.empty().to_tribool()(strng)), false, "not empty");
    test.check(
      _1.and_then(_1.empty().to_optional())
        .to_tribool()(opt)
        .is(eagine::indeterminate),
      "indeterminate");
}
//------------------------------------------------------------------------------
void placeholder_dereference(auto& s) {
    eagitest::case_ test{s, 9, "operator*"};
    using eagine::_1;

    std::optional<std::string> strng{"eagine.core"};
    test.check_equal((*_1)(strng), "eagine.core", "dereferenced value");
}
//------------------------------------------------------------------------------
void placeholder_ostream(auto& s) {
    eagitest::case_ test{s, 10, "operator<<"};
    using eagine::_1;
    using eagine::_2;
    using eagine::_3;

    std::stringstream temp;
    std::string s1{"eagine"};
    std::string s2{"core"};
    std::string s3{"types"};

    const std::string dot{"."};
    (temp << _1 << dot << _2 << dot << _3)(s1, s2, s3);
    test.check_equal(temp.str(), "eagine.core.types", "ostream value");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "placeholder expression", 10};
    test.once(placeholder_member_object);
    test.once(placeholder_member_function);
    test.once(placeholder_empty);
    test.once(placeholder_size);
    test.once(placeholder_has_value);
    test.once(placeholder_value_or);
    test.once(placeholder_and_then);
    test.once(placeholder_tribool);
    test.once(placeholder_dereference);
    test.once(placeholder_ostream);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
