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
    test.check(not loc.has_value(), "is not valid");
    test.check(not loc, "is false");
    test.check(not bool(loc), "is not true");
    test.check(not loc.scheme().has_value(), "invalid scheme");
    test.check(not loc.domain().has_value(), "invalid domain");
    test.check(not loc.login().has_value(), "invalid login");
    test.check(not loc.password().has_value(), "invalid login");
    test.check(not loc.path_str().has_value(), "invalid path string");
    test.check(not loc.port_str().has_value(), "invalid port string");
    test.check(not loc.port().has_value(), "invalid port number");
    test.check(not loc.fragment().has_value(), "invalid fragment");
    test.check(not loc.query_str().has_value(), "invalid query string");
    test.check(loc.query().empty(), "empty query argument map");
}
//------------------------------------------------------------------------------
void url_scheme(auto& s) {
    eagitest::case_ test{s, 2, "scheme"};

    const eagine::url l1{"//"};
    test.check(l1.has_value(), "1 valid");
    test.check(not l1.scheme().has_value(), "1");
    test.check_equal(l1.scheme().value_or("N/A"), "N/A", "1 matches");

    const eagine::url l2{"https://"};
    test.check(l2.scheme().has_value(), "2");
    test.check_equal(l2.scheme().value_or("N/A"), "https", "2 matches");

    const eagine::url l3{"file:///tmp"};
    test.check(l3.scheme().has_value(), "3");
    test.check_equal(l3.scheme().value_or("N/A"), "file", "3 matches");

    const eagine::url l4{"ftp://server/"};
    test.check(l4.scheme().has_value(), "4");
    test.check_equal(l4.scheme().value_or("N/A"), "ftp", "4 matches");

    const eagine::url l5{"sftp://user@server/path/to/file.txt"};
    test.check(l5.scheme().has_value(), "5");
    test.check_equal(l5.scheme().value_or("N/A"), "sftp", "5 matches");

    const eagine::url l6{"smb://user:passw0rd1@server/path/to/dir#part"};
    test.check(l6.scheme().has_value(), "6");
    test.check_equal(l6.scheme().value_or("N/A"), "smb", "6 matches");
}
//------------------------------------------------------------------------------
void url_domain(auto& s) {
    eagitest::case_ test{s, 3, "domain"};

    const eagine::url l1{"//"};
    test.check(l1.has_value(), "1 valid");
    test.check(not l1.domain().has_value(), "1");
    test.check_equal(l1.domain().value_or("N/A"), "N/A", "1 fallback");

    const eagine::url l2{"https://"};
    test.check(l2.has_value(), "2 valid");
    test.check(not l2.domain().has_value(), "2");
    test.check_equal(l2.domain().value_or("N/A"), "N/A", "2 fallback");

    const eagine::url l3{"file:///tmp"};
    test.check(l3.has_value(), "3 valid");
    test.check(not l3.domain().has_value(), "3");
    test.check_equal(l3.domain().value_or("N/A"), "N/A", "3 fallback");

    const eagine::url l4{"ftp://server1/"};
    test.check(l4.domain().has_value(), "4");
    test.check_equal(l4.domain().value_or("N/A"), "server1", "4 matches");

    const eagine::url l5{"sftp://user@server2/path/to/file.txt"};
    test.check(l5.domain().has_value(), "5");
    test.check_equal(l5.domain().value_or("N/A"), "server2", "5 matches");

    const eagine::url l6{"smb://user:passw0rd1@server3/path/to/dir#part"};
    test.check(l6.domain().has_value(), "6");
    test.check_equal(l6.domain().value_or("N/A"), "server3", "6 matches");
}
//------------------------------------------------------------------------------
void url_login(auto& s) {
    eagitest::case_ test{s, 4, "login"};

    const eagine::url l1{"//"};
    test.check(l1.has_value(), "1 valid");
    test.check(not l1.login().has_value(), "1");
    test.check_equal(l1.login().value_or("N/A"), "N/A", "1 fallback");

    const eagine::url l2{"https://"};
    test.check(l2.has_value(), "2 valid");
    test.check(not l2.login().has_value(), "2");
    test.check_equal(l2.login().value_or("N/A"), "N/A", "2 fallback");

    const eagine::url l3{"file:///tmp"};
    test.check(l3.has_value(), "3 valid");
    test.check(not l3.login().has_value(), "3");
    test.check_equal(l3.login().value_or("N/A"), "N/A", "3 fallback");

    const eagine::url l4{"ftp://server1/#frag"};
    test.check(l4.has_value(), "4 valid");
    test.check(not l4.login().has_value(), "4");
    test.check_equal(l4.login().value_or("N/A"), "N/A", "4 fallback");

    const eagine::url l5{"sftp://user1@server2/path/to/file.txt"};
    test.check(l5.login().has_value(), "5");
    test.check_equal(l5.login().value_or("N/A"), "user1", "5 matches");

    const eagine::url l6{"smb://user2:passw0rd1@server3/path/to/dir#part"};
    test.check(l6.login().has_value(), "6");
    test.check_equal(l6.login().value_or("N/A"), "user2", "6 matches");

    const eagine::url l7{"ldap://user3:pwd@server4:1234"};
    test.check(l7.login().has_value(), "7");
    test.check_equal(l7.login().value_or("N/A"), "user3", "7 matches");
}
//------------------------------------------------------------------------------
void url_password(auto& s) {
    eagitest::case_ test{s, 5, "password"};

    const eagine::url l1{"//"};
    test.check(l1.has_value(), "1 valid");
    test.check(not l1.password().has_value(), "1");
    test.check_equal(l1.password().value_or("N/A"), "N/A", "1 fallback");

    const eagine::url l2{"https://"};
    test.check(l2.has_value(), "2 valid");
    test.check(not l2.password().has_value(), "2");
    test.check_equal(l2.password().value_or("N/A"), "N/A", "2 fallback");

    const eagine::url l3{"file:///tmp"};
    test.check(l3.has_value(), "3 valid");
    test.check(not l3.password().has_value(), "3");
    test.check_equal(l3.password().value_or("N/A"), "N/A", "3 fallback");

    const eagine::url l4{"ftp://server1/#frag"};
    test.check(l4.has_value(), "4 valid");
    test.check(not l4.password().has_value(), "4");
    test.check_equal(l4.password().value_or("N/A"), "N/A", "4 fallback");

    const eagine::url l5{"sftp://user1@server2/path/to/file.txt"};
    test.check(l5.has_value(), "5 valid");
    test.check(not l5.password().has_value(), "5");
    test.check_equal(l5.password().value_or("N/A"), "N/A", "5 fallback");

    const eagine::url l6{"smb://user2:passw0rd1@server3/path/to/dir#part"};
    test.check(l6.password().has_value(), "6");
    test.check_equal(l6.password().value_or("N/A"), "passw0rd1", "6 matches");

    const eagine::url l7{"ldap://user3:pwd@server4:1234"};
    test.check(l7.password().has_value(), "7");
    test.check_equal(l7.password().value_or("N/A"), "pwd", "7 matches");
}
//------------------------------------------------------------------------------
void url_path_str(auto& s) {
    eagitest::case_ test{s, 6, "path string"};

    const eagine::url l1{"//"};
    test.check(l1.has_value(), "1 valid");
    test.check(not l1.path_str().has_value(), "1");
    test.check_equal(l1.path_str().value_or("N/A"), "N/A", "1 fallback");

    const eagine::url l2{"https://"};
    test.check(l2.has_value(), "2 valid");
    test.check(not l2.path_str().has_value(), "2");
    test.check_equal(l2.path_str().value_or("N/A"), "N/A", "2 fallback");

    const eagine::url l3{"file:///tmp"};
    test.check(l3.path_str().has_value(), "3");
    test.check_equal(l3.path_str().value_or("N/A"), "/tmp", "3 matches");

    const eagine::url l4{"ftp://server1/#frag"};
    test.check(l4.path_str().has_value(), "4");
    test.check_equal(l4.path_str().value_or("N/A"), "/", "4 matches");

    const eagine::url l5{"sftp://user1@server2/path/to/file.txt"};
    test.check(l5.path_str().has_value(), "5");
    test.check_equal(
      l5.path_str().value_or("N/A"), "/path/to/file.txt", "5 matches");

    const eagine::url l6{"smb://user2:passw0rd1@server3/path/to/dir#part"};
    test.check(l6.path_str().has_value(), "6");
    test.check_equal(
      l6.path_str().value_or("N/A"), "/path/to/dir", "6 matches");

    const eagine::url l7{"ldap://user3:pwd@server4:1234"};
    test.check(l7.has_value(), "7 valid");
    test.check(not l7.path_str().has_value(), "7");
    test.check_equal(l7.path_str().value_or("N/A"), "N/A", "7 fallback");
}
//------------------------------------------------------------------------------
void url_port_str(auto& s) {
    eagitest::case_ test{s, 7, "port string"};

    const eagine::url l1{"//"};
    test.check(l1.has_value(), "1 valid");
    test.check(not l1.port_str().has_value(), "1");
    test.check_equal(l1.port_str().value_or("N/A"), "N/A", "1 fallback");

    const eagine::url l2{"https://"};
    test.check(l2.has_value(), "2 valid");
    test.check(not l2.port_str().has_value(), "2");
    test.check_equal(l2.port_str().value_or("N/A"), "N/A", "2 fallback");

    const eagine::url l3{"file:///tmp"};
    test.check(l3.has_value(), "3 valid");
    test.check(not l3.port_str().has_value(), "3");
    test.check_equal(l3.port_str().value_or("N/A"), "N/A", "3 fallback");

    const eagine::url l4{"ftp://server1:42/#frag"};
    test.check(l4.port_str().has_value(), "4");
    test.check_equal(l4.port_str().value_or("N/A"), "42", "4 matches");

    const eagine::url l5{"sftp://user1@server2/path/to/file.txt"};
    test.check(not l5.port_str().has_value(), "5");
    test.check_equal(l5.port_str().value_or("N/A"), "N/A", "5 matches");

    const eagine::url l6{"smb://user2:passw0rd1@server3:69/path/to/dir#part"};
    test.check(l6.port_str().has_value(), "6");
    test.check_equal(l6.port_str().value_or("N/A"), "69", "6 matches");

    const eagine::url l7{"ldap://user3:pwd@server4:1234"};
    test.check(l7.port_str().has_value(), "7");
    test.check_equal(l7.port_str().value_or("N/A"), "1234", "7 matches");
}
//------------------------------------------------------------------------------
void url_port(auto& s) {
    eagitest::case_ test{s, 8, "port number"};

    const eagine::url l1{"//"};
    test.check(l1.has_value(), "1 valid");
    test.check(not l1.port().has_value(), "1");
    test.check_equal(l1.port().value_or(1), 1, "1 fallback");

    const eagine::url l2{"https://"};
    test.check(l2.has_value(), "2 valid");
    test.check(not l2.port().has_value(), "2");
    test.check_equal(l2.port().value_or(0), 0, "2 fallback");

    const eagine::url l3{"file:///tmp"};
    test.check(l3.has_value(), "3 valid");
    test.check(not l3.port().has_value(), "3");
    test.check_equal(l3.port().value_or(-1), -1, "3 fallback");

    const eagine::url l4{"ftp://server1:42/#frag"};
    test.check(l4.port().has_value(), "4");
    test.check_equal(l4.port().value_or(-1), 42, "4 matches");

    const eagine::url l5{"sftp://user1@server2/path/to/file.txt"};
    test.check(not l5.port().has_value(), "5");
    test.check_equal(l5.port().value_or(0), 0, "5 matches");

    const eagine::url l6{"smb://user2:passw0rd1@server3:69/path/to/dir#part"};
    test.check(l6.port().has_value(), "6");
    test.check_equal(l6.port().value_or(-1), 69, "6 matches");

    const eagine::url l7{"ldap://user3:pwd@server4:1234"};
    test.check(l7.port().has_value(), "7");
    test.check_equal(l7.port().value_or(0), 1234, "7 matches");
}
//------------------------------------------------------------------------------
void url_query_str(auto& s) {
    eagitest::case_ test{s, 9, "query string"};

    const eagine::url l1{"//"};
    test.check(l1.has_value(), "1 valid");
    test.check(not l1.query_str().has_value(), "1");
    test.check_equal(l1.query_str().value_or("N/A"), "N/A", "1 fallback");

    const eagine::url l2{"https://"};
    test.check(l2.has_value(), "2 valid");
    test.check(not l2.query_str().has_value(), "2");
    test.check_equal(l2.query_str().value_or("N/A"), "N/A", "2 fallback");

    const eagine::url l3{"file:///tmp"};
    test.check(l3.has_value(), "3 valid");
    test.check(not l3.query_str().has_value(), "3");
    test.check_equal(l3.query_str().value_or("N/A"), "N/A", "3 fallback");

    const eagine::url l4{"ftp://server1:42/?arg=val#frag"};
    test.check(l4.has_value(), "4 valid");
    test.check(l4.query_str().has_value(), "4");
    test.check_equal(l4.query_str().value_or("N/A"), "arg=val", "4 matches");

    const eagine::url l5{"sftp://user1@server2/path/to/file.txt"};
    test.check(l5.has_value(), "5 valid");
    test.check(not l5.query_str().has_value(), "5");
    test.check_equal(l5.query_str().value_or("N/A"), "N/A", "5 fallback");

    const eagine::url l6{"smb://user2:passw0rd1@server3:69/path/to/dir#part"};
    test.check(l6.has_value(), "6 valid");
    test.check(not l6.query_str().has_value(), "6");
    test.check_equal(l6.query_str().value_or("N/A"), "N/A", "6 fallback");

    const eagine::url l7{"ldap://user3:pwd@server4:1234?a=1+b=2+c=3#frag"};
    test.check(l7.query_str().has_value(), "7");
    test.check_equal(
      l7.query_str().value_or("N/A"), "a=1+b=2+c=3", "7 matches");
}
//------------------------------------------------------------------------------
void url_fragment(auto& s) {
    eagitest::case_ test{s, 10, "fragment"};

    const eagine::url l1{"//"};
    test.check(l1.has_value(), "1 valid");
    test.check(not l1.fragment().has_value(), "1");
    test.check_equal(l1.fragment().value_or("N/A"), "N/A", "1 fallback");

    const eagine::url l2{"https://"};
    test.check(l2.has_value(), "2 valid");
    test.check(not l2.fragment().has_value(), "2");
    test.check_equal(l2.fragment().value_or("N/A"), "N/A", "2 fallback");

    const eagine::url l3{"file:///tmp"};
    test.check(l3.has_value(), "3 valid");
    test.check(not l3.fragment().has_value(), "3");
    test.check_equal(l3.fragment().value_or("N/A"), "N/A", "3 fallback");

    const eagine::url l4{"ftp://server1:42/#frag"};
    test.check(l4.fragment().has_value(), "4");
    test.check_equal(l4.fragment().value_or("N/A"), "frag", "4 matches");

    const eagine::url l5{"sftp://user1@server2/path/to/file.txt"};
    test.check(not l5.fragment().has_value(), "5");
    test.check_equal(l5.fragment().value_or("N/A"), "N/A", "5 matches");

    const eagine::url l6{"smb://user2:passw0rd1@server3:69/path/to/dir#part"};
    test.check(l6.fragment().has_value(), "6");
    test.check_equal(l6.fragment().value_or("N/A"), "part", "6 matches");

    const eagine::url l7{"ldap://user3:pwd@server4:1234#root"};
    test.check(l7.fragment().has_value(), "7");
    test.check_equal(l7.fragment().value_or("N/A"), "root", "7 matches");
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "url", 10};
    test.once(url_default_construct);
    test.once(url_scheme);
    test.once(url_domain);
    test.once(url_login);
    test.once(url_password);
    test.once(url_path_str);
    test.once(url_port_str);
    test.once(url_port);
    test.once(url_query_str);
    test.once(url_fragment);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
