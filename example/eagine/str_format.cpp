/// @example eagine/str_format.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import std;

auto main() -> int {
    using namespace eagine;
    using s = std::string;
    using std::cout;
    using std::endl;

    cout << s(format("prefix-${1}-suffix") % "BLAH") << endl;
    cout << s(format("${2}${1}${4}${3}${1}") % "foo" % "bar" % "baz" % "qux")
         << endl;

    s dst;
    (format("${2}${1}${4}${3}${1}") % "foo" % "bar" % "baz" % "qux").into(dst);
    std::cout << dst << std::endl;

    return 0;
}
