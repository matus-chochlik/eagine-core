/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.types;
import eagine.core.utility;
//------------------------------------------------------------------------------
void scope_exit_false_type(auto& s) {
    eagitest::case_ test{s, 1, "false_type"};
    eagitest::track trck{test, 4, 4};

    int i = 10;
    const auto inc_i = [&i]() {
        ++i;
    };

    {
        test.check_equal(i, 10, "A entry");
        eagine::on_scope_exit<std::false_type> se(inc_i);
        test.check_equal(i, 10, "A inside");
        trck.checkpoint(1);
    }
    test.check_equal(i, 11, "A exit");

    {
        test.check_equal(i, 11, "B entry");
        eagine::on_scope_exit<std::false_type> se(inc_i);
        test.check_equal(i, 11, "B inside");
        trck.checkpoint(2);
        se.cancel();
    }
    try {
        test.check_equal(i, 11, "C entry");
        eagine::on_scope_exit<std::false_type> se(inc_i);
        test.check_equal(i, 11, "C inside");
        trck.checkpoint(3);

        throw std::exception();
    } catch(const std::exception&) {
        test.check_equal(i, 11, "C exit");
        trck.checkpoint(4);
    }
}
//------------------------------------------------------------------------------
void scope_exit_true_type(auto& s) {
    eagitest::case_ test{s, 2, "true_type"};
    eagitest::track trck{test, 5, 5};

    int i = 11;
    const auto inc_i = [&i]() {
        ++i;
    };

    {
        test.check_equal(i, 11, "A entry");
        eagine::on_scope_exit<std::true_type> se(inc_i);
        test.check_equal(i, 11, "A inside");
        trck.checkpoint(1);
    }
    test.check_equal(i, 11, "A exit");

    try {
        test.check_equal(i, 11, "B entry");
        eagine::on_scope_exit<std::true_type> se(inc_i);
        test.check_equal(i, 11, "B inside");
        trck.checkpoint(2);

        throw std::exception();
    } catch(const std::exception&) {
        test.check_equal(i, 12, "B catch");
        trck.checkpoint(3);
    }

    try {
        test.check_equal(i, 12, "C entry");
        eagine::on_scope_exit<std::true_type> se(inc_i);
        test.check_equal(i, 12, "C inside");
        trck.checkpoint(4);
        se.cancel();

        throw std::exception();
    } catch(const std::exception&) {
        test.check_equal(i, 12, "C catch");
        trck.checkpoint(5);
    }
}
//------------------------------------------------------------------------------
void scope_exit_nothing(auto& s) {
    eagitest::case_ test{s, 3, "nothing"};
    eagitest::track trck{test, 6, 6};

    int i = 41;
    const auto inc_i = [&i]() {
        ++i;
    };

    {
        test.check_equal(i, 41, "A entry");
        eagine::on_scope_exit<eagine::nothing_t> se(inc_i);
        test.check_equal(i, 41, "A inside");
        trck.checkpoint(1);
    }
    test.check_equal(i, 42, "A exit");

    {
        test.check_equal(i, 42, "B entry");
        eagine::on_scope_exit<eagine::nothing_t> se(inc_i);
        test.check_equal(i, 42, "B inside");
        trck.checkpoint(2);
        se.cancel();
    }
    test.check_equal(i, 42, "B exit");

    try {
        test.check_equal(i, 42, "C entry");
        eagine::on_scope_exit<eagine::nothing_t> se(inc_i);
        test.check_equal(i, 42, "C entry");
        trck.checkpoint(3);

        throw std::exception();
    } catch(const std::exception&) {
        test.check_equal(i, 43, "C catch");
        trck.checkpoint(4);
    }
    try {
        test.check_equal(i, 43, "D entry");
        eagine::on_scope_exit<eagine::nothing_t> se(inc_i);
        test.check_equal(i, 43, "D inside");
        trck.checkpoint(5);
        se.cancel();

        throw std::exception();
    } catch(const std::exception&) {
        test.check_equal(i, 43, "D catch");
        trck.checkpoint(6);
    }
}
//------------------------------------------------------------------------------
void scope_exit_finally(auto& s) {
    eagitest::case_ test{s, 4, "finally"};
    eagitest::track trck{test, 6, 6};

    int i = 41;

    {
        test.check_equal(i, 41, "A entry");
        auto fse = eagine::finally([&i]() { i += 2; });
        test.check_equal(i, 41, "A inside");
        trck.checkpoint(1);
    }
    test.check_equal(i, 43, "A exit");

    {
        test.check_equal(i, 43, "B entry");
        auto fse = eagine::finally([&i]() { i += 2; });
        test.check_equal(i, 43, "B inside");
        trck.checkpoint(2);
        fse.cancel();
    }
    test.check_equal(i, 43, "B exit");

    try {
        test.check_equal(i, 43, "C entry");
        auto fse = eagine::finally([&i]() { --i; });
        test.check_equal(i, 43, "C inside");
        trck.checkpoint(3);

        throw std::exception();
    } catch(const std::exception&) {
        test.check_equal(i, 42, "C catch");
        trck.checkpoint(4);
    }
    try {
        test.check_equal(i, 42, "D entry");
        auto fse = eagine::finally([&i]() { --i; });
        test.check_equal(i, 42, "D inside");
        trck.checkpoint(5);
        fse.cancel();

        throw std::exception();
    } catch(const std::exception&) {
        test.check_equal(i, 42, "D catch");
        trck.checkpoint(6);
    }
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "scope_exit", 4};
    test.once(scope_exit_false_type);
    test.once(scope_exit_true_type);
    test.once(scope_exit_nothing);
    test.once(scope_exit_finally);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
