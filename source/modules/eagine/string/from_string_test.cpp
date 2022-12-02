/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.types;
import eagine.core.string;
//------------------------------------------------------------------------------
void from_string_int(auto& s) {
    using eagine::extract;
    using eagine::from_string;
    eagitest::case_ test{s, 1, "int"};
    eagitest::track trck{test, 6, 6};

    if(const auto conv{from_string<int>("0")}) {
        test.check_equal(extract(conv), 0, "0");
        trck.passed_part(1);
    }

    if(const auto conv{from_string<int>("-1")}) {
        test.check_equal(extract(conv), -1, "-1");
        trck.passed_part(2);
    }

    if(const auto conv{from_string<int>("1")}) {
        test.check_equal(extract(conv), 1, "1");
        trck.passed_part(3);
    }

    if(const auto conv{from_string<int>("2.34")}) {
        (void)conv;
    } else {
        trck.passed_part(4);
    }

    if(const auto conv{from_string<int>("12345")}) {
        test.check_equal(extract(conv), 12345, "12345");
        trck.passed_part(5);
    }

    if(const auto conv{from_string<int>("ABC")}) {
        (void)conv;
    } else {
        trck.passed_part(6);
    }
}
//------------------------------------------------------------------------------
void from_string_unsigned(auto& s) {
    using eagine::extract;
    using eagine::from_string;
    eagitest::case_ test{s, 2, "unsigned"};
    eagitest::track trck{test, 6, 6};

    if(const auto conv{from_string<unsigned>("0")}) {
        test.check_equal(extract(conv), 0U, "0");
        trck.passed_part(1);
    }

    if(const auto conv{from_string<unsigned>("-1")}) {
        (void)conv;
    } else {
        trck.passed_part(2);
    }

    if(const auto conv{from_string<unsigned>("1")}) {
        test.check_equal(extract(conv), 1U, "1");
        trck.passed_part(3);
    }

    if(const auto conv{from_string<unsigned>("2.34")}) {
        (void)conv;
    } else {
        trck.passed_part(4);
    }

    if(const auto conv{from_string<unsigned>("12345")}) {
        test.check_equal(extract(conv), 12345U, "12345");
        trck.passed_part(5);
    }

    if(const auto conv{from_string<unsigned>("ABC")}) {
        (void)conv;
    } else {
        trck.passed_part(6);
    }
}
//------------------------------------------------------------------------------
void from_string_float(auto& s) {
    using eagine::extract;
    using eagine::from_string;
    eagitest::case_ test{s, 3, "float"};
    eagitest::track trck{test, 7, 7};

    if(const auto conv{from_string<float>("0.0")}) {
        test.check_equal(extract(conv), 0.F, "0");
        trck.passed_part(1);
    }

    if(const auto conv{from_string<float>("-1.0")}) {
        test.check_close(extract(conv), -1.F, "-1");
        trck.passed_part(2);
    }

    if(const auto conv{from_string<float>("1.0")}) {
        test.check_close(extract(conv), 1.F, "1");
        trck.passed_part(3);
    }

    if(const auto conv{from_string<float>("12345")}) {
        test.check_equal(extract(conv), 12345.F, "12345");
        trck.passed_part(4);
    }

    if(const auto conv{from_string<float>("-12345")}) {
        test.check_equal(extract(conv), -12345.F, "-12345");
        trck.passed_part(5);
    }

    if(const auto conv{from_string<float>("12345.6789")}) {
        test.check_equal(extract(conv), 12345.6789F, "12345.6789");
        trck.passed_part(6);
    }

    if(const auto conv{from_string<float>("ABC")}) {
        (void)conv;
    } else {
        trck.passed_part(7);
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
