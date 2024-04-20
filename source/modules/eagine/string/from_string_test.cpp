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
void from_string_int(auto& s) {
    using eagine::_1;
    using eagine::from_string;
    eagitest::case_ test{s, 1, "int"};
    eagitest::track trck{test, 7, 7};

    if(const auto conv{from_string<int>("0")}) {
        test.check_equal(*conv, 0, "0");
        trck.checkpoint(1);
    }

    if(const auto conv{from_string<int>("-1")}) {
        test.check_equal(*conv, -1, "-1");
        trck.checkpoint(2);
    }

    if(const auto conv{from_string<int>("1")}) {
        test.check_equal(*conv, 1, "1");
        trck.checkpoint(3);
    }

    if(const auto conv{from_string<int>("2.34")}) {
        (void)conv;
    } else {
        trck.checkpoint(4);
    }

    if(const auto conv{from_string<int>("12345")}) {
        test.check_equal(*conv, 12345, "12345");
        trck.checkpoint(5);
    }

    if(const auto conv{from_string<int>(_1)("23456")}) {
        test.check_equal(*conv, 23456, "23456");
        trck.checkpoint(6);
    }

    if(const auto conv{from_string<int>("ABC")}) {
        (void)conv;
    } else {
        trck.checkpoint(7);
    }
}
//------------------------------------------------------------------------------
void from_string_unsigned(auto& s) {
    using eagine::_2;
    using eagine::from_string;
    eagitest::case_ test{s, 2, "unsigned"};
    eagitest::track trck{test, 7, 7};

    if(const auto conv{from_string<unsigned>("0")}) {
        test.check_equal(*conv, 0U, "0");
        trck.checkpoint(1);
    }

    if(const auto conv{from_string<unsigned>("-1")}) {
        (void)conv;
    } else {
        trck.checkpoint(2);
    }

    if(const auto conv{from_string<unsigned>("1")}) {
        test.check_equal(*conv, 1U, "1");
        trck.checkpoint(3);
    }

    if(const auto conv{from_string<unsigned>("2.34")}) {
        (void)conv;
    } else {
        trck.checkpoint(4);
    }

    if(const auto conv{from_string<unsigned>("12345")}) {
        test.check_equal(*conv, 12345U, "12345");
        trck.checkpoint(5);
    }

    if(const auto conv{from_string<unsigned>(_2)("12345", "23456")}) {
        test.check_equal(*conv, 23456U, "23456");
        trck.checkpoint(6);
    }

    if(const auto conv{from_string<unsigned>("ABC")}) {
        (void)conv;
    } else {
        trck.checkpoint(7);
    }
}
//------------------------------------------------------------------------------
void from_string_float(auto& s) {
    using eagine::_3;
    using eagine::from_string;
    eagitest::case_ test{s, 3, "float"};
    eagitest::track trck{test, 8, 8};

    if(const auto conv{from_string<float>("0.0")}) {
        test.check_equal(*conv, 0.F, "0");
        trck.checkpoint(1);
    }

    if(const auto conv{from_string<float>("-1.0")}) {
        test.check_close(*conv, -1.F, "-1");
        trck.checkpoint(2);
    }

    if(const auto conv{from_string<float>("1.0")}) {
        test.check_close(*conv, 1.F, "1");
        trck.checkpoint(3);
    }

    if(const auto conv{from_string<float>("12345")}) {
        test.check_equal(*conv, 12345.F, "12345");
        trck.checkpoint(4);
    }

    if(const auto conv{from_string<float>("-12345")}) {
        test.check_equal(*conv, -12345.F, "-12345");
        trck.checkpoint(5);
    }

    if(const auto conv{from_string<float>("12345.6789")}) {
        test.check_equal(*conv, 12345.6789F, "12345.6789");
        trck.checkpoint(6);
    }

    if(const auto conv{from_string<float>(_3)(1, 2, "1234.567")}) {
        test.check_equal(*conv, 1234.567F, "1234.567");
        trck.checkpoint(7);
    }

    if(const auto conv{from_string<float>("ABC")}) {
        (void)conv;
    } else {
        trck.checkpoint(8);
    }
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "from_string", 3};
    test.once(from_string_int);
    test.once(from_string_unsigned);
    test.once(from_string_float);
    // TODO
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
