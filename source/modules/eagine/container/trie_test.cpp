/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.types;
import eagine.core.container;
import <set>;
import <string>;
//------------------------------------------------------------------------------
void trie_empty(auto& s) {
    eagitest::case_ test{s, 1, "empty"};
    auto& rg{test.random()};
    eagine::identifier_trie t;

    for(unsigned i = 0; i < test.repeats(10000); ++i) {
        test.check(
          !t.contains(rg.get_string_made_of(1, 50, t.valid_chars())),
          "does not contain");
    }
}
//------------------------------------------------------------------------------
void trie_insert_1(auto& s) {
    eagitest::case_ test{s, 2, "insert 1"};
    auto& rg{test.random()};
    eagine::identifier_trie t;
    std::set<std::string> b;

    for(unsigned i = 0; i < test.repeats(10000); ++i) {
        auto key{rg.get_string_made_of(1, 50, t.valid_chars())};
        b.insert(key);
        test.check(t.insert(key), "insert ok");
    }

    for(const auto& key : b) {
        test.check(t.contains(key), key);
    }

    for(unsigned i = 0; i < test.repeats(10000); ++i) {
        auto key{rg.get_string_made_of(1, 50, t.valid_chars())};
        test.check_equal(t.contains(key), b.contains(key), "contains ok");
    }
}
//------------------------------------------------------------------------------
void trie_insert_2(auto& s) {
    eagitest::case_ test{s, 3, "insert 2"};
    eagitest::track trck{test, 0, 1};
    auto& rg{test.random()};
    eagine::basic_identifier_trie<std::string> t;
    std::set<std::string> b;

    for(unsigned i = 0; i < test.repeats(10000); ++i) {
        auto key{rg.get_string_made_of(1, 50, t.valid_chars())};
        b.insert(key);
        test.check(t.insert(key, key), "insert ok");
    }

    for(const auto& key : b) {
        test.check(t.contains(key), key);
        if(const auto found{t.find(key)}) {
            test.check_equal(extract(found), key, "key == value");
            trck.passed_part(1);
        }
    }

    for(unsigned i = 0; i < test.repeats(10000); ++i) {
        auto key{rg.get_string_made_of(1, 50, t.valid_chars())};
        test.check_equal(t.contains(key), b.contains(key), "contains ok");
    }
}
//------------------------------------------------------------------------------
void trie_add_1(auto& s) {
    eagitest::case_ test{s, 4, "add 1"};
    auto& rg{test.random()};
    eagine::identifier_trie t;
    std::set<std::string> b;

    for(unsigned i = 0; i < test.repeats(10000); ++i) {
        auto key{rg.get_string_made_of(1, 50, t.valid_chars())};
        b.insert(key);
        t.add(key);
    }

    for(const auto& key : b) {
        test.check(t.contains(key), key);
    }

    for(unsigned i = 0; i < test.repeats(10000); ++i) {
        auto key{rg.get_string_made_of(1, 50, t.valid_chars())};
        test.check_equal(t.contains(key), b.contains(key), "contains ok");
    }
}
//------------------------------------------------------------------------------
void trie_add_2(auto& s) {
    eagitest::case_ test{s, 5, "add 2"};
    eagitest::track trck{test, 0, 1};
    auto& rg{test.random()};
    eagine::basic_identifier_trie<std::string> t;
    std::set<std::string> b;

    for(unsigned i = 0; i < test.repeats(10000); ++i) {
        auto key{rg.get_string_made_of(1, 50, t.valid_chars())};
        b.insert(key);
        t.add(key, key);
    }

    for(const auto& key : b) {
        test.check(t.contains(key), key);
        if(const auto found{t.find(key)}) {
            test.check_equal(extract(found), key, "key == value");
            trck.passed_part(1);
        }
    }

    for(unsigned i = 0; i < test.repeats(10000); ++i) {
        auto key{rg.get_string_made_of(1, 50, t.valid_chars())};
        test.check_equal(t.contains(key), b.contains(key), "contains ok");
    }
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "trie", 5};
    test.once(trie_empty);
    test.once(trie_insert_1);
    test.once(trie_insert_2);
    test.once(trie_add_1);
    test.once(trie_add_2);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
