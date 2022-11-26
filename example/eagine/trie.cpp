/// @example eagine/trie.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import <iostream>;

auto main() -> int {
    using namespace eagine;

    auto print = [](const auto& opt) {
        if(opt) {
            std::cout << extract(opt) << std::endl;
        } else {
            std::cout << "<none>" << std::endl;
        }
    };

    basic_identifier_trie<int> t;

    print(t.find("foo"));
    t.insert("foo", 0);
    print(t.find("foo"));
    print(t.find("bar"));
    t.insert("bar", 1);
    print(t.find("foo"));
    print(t.find("bar"));
    print(t.find("baz"));
    t.insert("baz", 2);
    print(t.find("foo"));
    print(t.find("bar"));
    print(t.find("baz"));
    print(t.find("qux"));
    t.insert("qux", 3);
    print(t.find("foo"));
    print(t.find("bar"));
    print(t.find("baz"));
    print(t.find("qux"));

    return 0;
}
