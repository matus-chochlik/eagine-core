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

    t.traverse([](auto key, const auto& val) {
        std::cout << key << ": " << val << std::endl;
    });
}

void stats_incr_dec() {
    using namespace eagine;

    int count = 1'000'000;
    while(count > 0) {
        basic_dec_identifier_trie<int> t;
        t.reserve(count + 1);
        int i = 0;
        while(i < count) {
            const auto id{dec_to_identifier(i)};
            t.insert(id.name(), i++);
        }
        std::cout << count << ": " << t.internal_node_count() << " + "
                  << t.value_node_count() << " = " << t.node_count() << " ("
                  << t.overhead() << ")" << std::endl;
        count /= 10;
    }
}

void stats_random_id() {
    using namespace eagine;
    std::random_device rd;
    std::default_random_engine re{rd()};

    int count = 1'000'000;
    while(count > 0) {
        basic_lc_identifier_trie<int> t;
        t.reserve(5 * count);
        int i = 0;
        while(i < count) {
            const auto id{random_identifier(t.valid_chars(), re)};
            if(not t.contains(id.name())) {
                t.insert(id.name(), i++);
            }
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
    stats_incr_dec();
    std::cout << "---" << std::endl;
    stats_random_id();

    return 0;
}
