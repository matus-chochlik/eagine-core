/// @example eagine/trie.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import std;

void basic_usage() {
    using namespace eagine;

    auto print = [](const auto& opt) {
        if(opt) {
            std::cout << *opt << std::endl;
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
    t.insert("ufo", 4);
    print(t.find("foo"));
    print(t.find("bar"));
    print(t.find("baz"));
    print(t.find("qux"));
    print(t.find("ufo"));
}

void stats() {
    using namespace eagine;

    int count = 1'000'000;
    while(count > 0) {
        basic_identifier_trie<int> t;
        t.reserve(10 * count);
        for(const auto i : integer_range(count)) {
            t.insert(random_identifier().name(), i);
        }
        std::cout << count << ": " << t.internal_node_count() << " + "
                  << t.value_node_count() << " = " << t.node_count() << " ("
                  << t.overhead() << ")" << std::endl;
        count /= 10;
    }
}

auto main() -> int {
    basic_usage();
    std::cout << "---" << std::endl;
    stats();

    return 0;
}
