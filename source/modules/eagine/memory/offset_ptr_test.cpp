/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import std;
import eagine.core.memory;
//------------------------------------------------------------------------------
void offset_ptr_default_construct(auto& s) {
    eagitest::case_ test{s, 1, "default construct"};
    using namespace eagine;

    memory::offset_ptr<double> ps;

    test.check_equal(bool(ps), false, "is false");
    test.check_equal(not ps, true, "is not true");
    test.check_equal(!!ps, false, "is not not false");

    test.check(ps.get() == nullptr, "equal to nullptr");
    test.check(ps.addr() == memory::const_address(), "default address");
    test.check(ps.addr().value() == 0, "zero address value");
    test.check(static_cast<double*>(ps) == ps.get(), "cast is ok");
}
//------------------------------------------------------------------------------
void offset_ptr_copy_construct_1(auto& s) {
    eagitest::case_ test{s, 2, "copy construct 1"};
    using namespace eagine;

    std::string str("blahbleh");

    memory::offset_ptr<const std::string> ps(&str);

    test.check_equal(bool(ps), true, "is true");
    test.check_equal(not ps, false, "is not false");
    test.check_equal(!!ps, true, "is not not true");

    test.ensure(ps.get() == &str, "get to original pointer");
    test.check(ps.addr() == memory::const_address(&str), "address is ok");
    test.check(static_cast<const std::string*>(ps) == ps.get(), "cast is ok");

    test.check_equal(*ps, str, "pointee is ok");
    test.check_equal(ps->size(), str.size(), "size is ok");
}
//------------------------------------------------------------------------------
void offset_ptr_copy_construct_2(auto& s) {
    eagitest::case_ test{s, 3, "copy construct 2"};
    using namespace eagine;

    std::string str("foobarbaz");
    memory::offset_ptr<std::string> ops(&str);

    memory::offset_ptr<std::string> ps(ops);

    test.check_equal(bool(ps), true, "is true");
    test.check_equal(not ps, false, "is not false");
    test.check_equal(!!ps, true, "is not not true");

    test.ensure(ps.get() == &str, "get to original pointer");
    test.check(ps.addr() == memory::const_address(&str), "address is ok");
    test.check(static_cast<const std::string*>(ps) == ps.get(), "cast is ok");

    test.check_equal(*ps, str, "pointee is ok");
    test.check_equal(ps->size(), str.size(), "size is ok");
}
//------------------------------------------------------------------------------
void offset_ptr_copy_construct_3(auto& s) {
    eagitest::case_ test{s, 4, "copy construct 3"};
    using namespace eagine;

    memory::offset_ptr<std::string> ops;

    memory::offset_ptr<std::string> ps(ops);

    test.check_equal(bool(ps), false, "is false");
    test.check_equal(not ps, true, "is not true");
    test.check_equal(!!ps, false, "is not not false");

    test.check(ps.get() == nullptr, "equal to nullptr");
    test.check(ps.addr() == memory::const_address(), "default address");
    test.check(ps.addr().value() == 0, "zero address value");
    test.check(static_cast<std::string*>(ps) == ps.get(), "cast is ok");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "offset_ptr", 4};
    test.once(offset_ptr_default_construct);
    test.once(offset_ptr_copy_construct_1);
    test.once(offset_ptr_copy_construct_2);
    test.once(offset_ptr_copy_construct_3);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
