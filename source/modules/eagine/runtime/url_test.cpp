/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.memory;
import eagine.core.runtime;
//------------------------------------------------------------------------------
void url_default_construct(auto& s) {
    eagitest::case_ test{s, 1, "default construct"};

    eagine::url loc;
    test.check(not loc.is_valid(), "is not valid");
    test.check(not loc, "is false");
    test.check(not bool(loc), "is not true");
    test.check(not loc.scheme().is_valid(), "invalid scheme");
    test.check(not loc.domain().is_valid(), "invalid domain");
    test.check(not loc.login().is_valid(), "invalid login");
    test.check(not loc.password().is_valid(), "invalid login");
    test.check(not loc.path_str().is_valid(), "invalid path string");
    test.check(not loc.port_str().is_valid(), "invalid port string");
    test.check(not loc.port().is_valid(), "invalid port number");
    test.check(not loc.fragment().is_valid(), "invalid fragment");
    test.check(not loc.query_str().is_valid(), "invalid query string");
    test.check(loc.query().empty(), "empty query argument map");
}
//------------------------------------------------------------------------------
void url_scheme(auto& s) {
    eagitest::case_ test{s, 2, "scheme"};

    const eagine::url l1{"//"};
    test.check(l1.is_valid(), "1 valid");
    test.check(not l1.scheme().is_valid(), "1");
    test.check_equal(l1.scheme().value_or("N/A"), "N/A", "1 matches");

    const eagine::url l2{"https://"};
    test.check(l2.scheme().is_valid(), "2");
    test.check_equal(l2.scheme().value_or("N/A"), "https", "2 matches");

    const eagine::url l3{"file:///tmp"};
    test.check(l3.scheme().is_valid(), "3");
    test.check_equal(l3.scheme().value_or("N/A"), "file", "3 matches");

    const eagine::url l4{"ftp://server/"};
    test.check(l4.scheme().is_valid(), "4");
    test.check_equal(l4.scheme().value_or("N/A"), "ftp", "4 matches");

    const eagine::url l5{"sftp://user@server/path/to/file.txt"};
    test.check(l5.scheme().is_valid(), "5");
    test.check_equal(l5.scheme().value_or("N/A"), "sftp", "5 matches");

    const eagine::url l6{"smb://user:passw0rd1@server/path/to/dir#part"};
    test.check(l6.scheme().is_valid(), "6");
    test.check_equal(l6.scheme().value_or("N/A"), "smb", "6 matches");
}
//------------------------------------------------------------------------------
void url_domain(auto& s) {
    eagitest::case_ test{s, 3, "domain"};

    const eagine::url l1{"//"};
    test.check(l1.is_valid(), "1 valid");
    test.check(not l1.domain().is_valid(), "1");
    test.check_equal(l1.domain().value_or("N/A"), "N/A", "1 fallback");

    const eagine::url l2{"https://"};
    test.check(l2.is_valid(), "2 valid");
    test.check(not l2.domain().is_valid(), "2");
    test.check_equal(l2.domain().value_or("N/A"), "N/A", "2 fallback");

    const eagine::url l3{"file:///tmp"};
    test.check(l3.is_valid(), "3 valid");
    test.check(not l3.domain().is_valid(), "3");
    test.check_equal(l3.domain().value_or("N/A"), "N/A", "3 fallback");

    const eagine::url l4{"ftp://server1/"};
    test.check(l4.domain().is_valid(), "4");
    test.check_equal(l4.domain().value_or("N/A"), "server1", "4 matches");

    const eagine::url l5{"sftp://user@server2/path/to/file.txt"};
    test.check(l5.domain().is_valid(), "5");
    test.check_equal(l5.domain().value_or("N/A"), "server2", "5 matches");

    const eagine::url l6{"smb://user:passw0rd1@server3/path/to/dir#part"};
    test.check(l6.domain().is_valid(), "6");
    test.check_equal(l6.domain().value_or("N/A"), "server3", "6 matches");
}
//------------------------------------------------------------------------------
void url_login(auto& s) {
    eagitest::case_ test{s, 4, "login"};

    const eagine::url l1{"//"};
    test.check(l1.is_valid(), "1 valid");
    test.check(not l1.login().is_valid(), "1");
    test.check_equal(l1.login().value_or("N/A"), "N/A", "1 fallback");

    const eagine::url l2{"https://"};
    test.check(l2.is_valid(), "2 valid");
    test.check(not l2.login().is_valid(), "2");
    test.check_equal(l2.login().value_or("N/A"), "N/A", "2 fallback");

    const eagine::url l3{"file:///tmp"};
    test.check(l3.is_valid(), "3 valid");
    test.check(not l3.login().is_valid(), "3");
    test.check_equal(l3.login().value_or("N/A"), "N/A", "3 fallback");

    const eagine::url l4{"ftp://server1/#frag"};
    test.check(l4.is_valid(), "4 valid");
    test.check(not l4.login().is_valid(), "4");
    test.check_equal(l4.login().value_or("N/A"), "N/A", "4 fallback");

    const eagine::url l5{"sftp://user1@server2/path/to/file.txt"};
    test.check(l5.login().is_valid(), "5");
    test.check_equal(l5.login().value_or("N/A"), "user1", "5 matches");

    const eagine::url l6{"smb://user2:passw0rd1@server3/path/to/dir#part"};
    test.check(l6.login().is_valid(), "6");
    test.check_equal(l6.login().value_or("N/A"), "user2", "6 matches");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "url", 4};
    test.once(url_default_construct);
    test.once(url_scheme);
    test.once(url_domain);
    test.once(url_login);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
