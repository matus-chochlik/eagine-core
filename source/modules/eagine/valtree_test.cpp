/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin_ctx.hpp>
import eagine.core;
//------------------------------------------------------------------------------
void compound_get_url(auto& s) {
    eagitest::case_ test{s, 1, "compound get_url"};

    using path = eagine::basic_string_path;
    const eagine::string_view json_text(R"({
		"URL1" : "sftp://user:pwd@oglplus.org/file.txt?days=1#fragment",
		"URL2" : {
			"scheme": "https",
			"login": "user",
			"password": "mypass123",
			"domain": "example.com",
			"port": 1234,
			"path": "/path/to/resource",
			"fragment": "bookmark",
			"args": {
				"id": 1234,
				"url": "ftp://example.com/path/to/file#part1"
			}
		}
	})");
    const auto vc{eagine::valtree::from_json_text(json_text, s.context())};

    test.check(bool(vc), "is true");

    auto url1{vc.get_url(path("URL1"))};
    test.ensure(bool(url1), "has URL1");

    test.check_equal(url1.scheme(), "sftp", "URL1 scheme");
    test.check_equal(url1.login(), "user", "URL1 login");
    test.check_equal(url1.password(), "pwd", "URL1 password");
    test.check_equal(url1.domain(), "oglplus.org", "URL1 domain");
    test.check_equal(url1.path_str(), "/file.txt", "URL1 path");
    test.check_equal(url1.fragment(), "fragment", "URL1 fragment");
    test.check_equal(url1.query().decoded_arg_value("days"), "1", "URL1 arg");

    auto url2{vc.get_url(path("URL2"))};
    test.ensure(bool(url2), "has URL2");
    test.check_equal(url2.scheme(), "https", "URL2 scheme");
    test.check_equal(url2.login(), "user", "URL2 login");
    test.check_equal(url2.password(), "mypass123", "URL2 password");
    test.check_equal(url2.domain(), "example.com", "URL2 domain");
    test.check_equal(url2.path_str(), "/path/to/resource", "URL2 path");
    test.check_equal(url2.fragment(), "bookmark", "URL2 fragment");
    test.check_equal(url2.query().decoded_arg_value("id"), "1234", "URL2 arg");

    auto url3{url2.query().arg_url("url")};
    test.ensure(bool(url3), "has URL3");
    test.check_equal(url3.scheme(), "ftp", "URL3 scheme");
    test.check_equal(url3.domain(), "example.com", "URL3 domain");
    test.check_equal(url3.path_str(), "/path/to/file", "URL3 path");
    test.check_equal(url3.fragment(), "part1", "URL3 fragment");
}
//------------------------------------------------------------------------------
auto test_main(eagine::test_ctx& ctx) -> int {
    eagitest::ctx_suite test{ctx, "value_tree", 1};
    test.once(compound_get_url);
    return test.exit_code();
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    return eagine::test_main_impl(argc, argv, test_main);
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end_ctx.hpp>

