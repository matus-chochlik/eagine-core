/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.container;
import <set>;
import <functional>;
//------------------------------------------------------------------------------
void flat_set_default_construct(auto& s) {
    eagitest::case_ test{s, "default construct"};
    eagine::flat_set<int> fs;

    test.check(fs.empty(), "is empty");
    test.check_equal(fs.size(), 0U, "size is zero");
    test.check(fs.begin() == fs.end(), "begin == end");
}
//------------------------------------------------------------------------------
void flat_set_init_from_vector(auto& s) {
    eagitest::case_ test{s, "init from vector"};
    eagitest::track trck{test, 2, 2};
    auto& rg{test.random()};

    std::set<int> ss;
    std::vector<int> d;

    for(unsigned i = 0; i < test.repeats(10000); ++i) {
        int k = rg.get_any<int>();
        ss.insert(k);
        d.push_back(k);
        trck.passed_part(1);
    }

    eagine::flat_set<int> fs(d);

    test.check_equal(ss.empty(), fs.empty(), "empty is same");
    test.check_equal(ss.size(), fs.size(), "size is same");

    auto ssi = ss.begin();
    auto fsi = fs.begin();

    while((ssi != ss.end()) && (fsi != fs.end())) {
        test.check_equal(*ssi, *fsi, "elements same");
        ++ssi;
        ++fsi;
        trck.passed_part(2);
    }

    test.check(ssi == ss.end(), "std::set iterator ok");
    test.check(fsi == fs.end(), "flat:set iterator ok");
}
//------------------------------------------------------------------------------
void flat_set_insert(auto& s) {
    eagitest::case_ test{s, "insert"};
    eagitest::track trck{test, 2, 2};
    auto& rg{test.random()};

    std::set<int> ss;
    eagine::flat_set<int> fs;

    for(unsigned i = 0; i < test.repeats(10000); ++i) {
        int k = rg.get_any<int>();
        ss.insert(k);
        fs.insert(k);
        trck.passed_part(1);
    }

    test.check_equal(ss.empty(), fs.empty(), "empty is same");
    test.check_equal(ss.size(), fs.size(), "size is same");

    auto ssi = ss.begin();
    auto fsi = fs.begin();

    while((ssi != ss.end()) && (fsi != fs.end())) {
        test.check_equal(*ssi, *fsi, "elements same");
        ++ssi;
        ++fsi;
        trck.passed_part(2);
    }

    test.check(ssi == ss.end(), "std::set iterator ok");
    test.check(fsi == fs.end(), "flat:set iterator ok");
}
//------------------------------------------------------------------------------
void flat_set_insert_at_begin(auto& s) {
    eagitest::case_ test{s, "insert at begin"};
    eagitest::track trck{test, 2, 2};
    auto& rg{test.random()};

    std::set<int> ss;
    eagine::flat_set<int> fs;

    for(unsigned i = 0; i < test.repeats(10000); ++i) {
        int k = rg.get_any<int>();
        ss.insert(ss.begin(), k);
        fs.insert(fs.begin(), k);
        trck.passed_part(1);
    }

    test.check_equal(ss.empty(), fs.empty(), "empty is same");
    test.check_equal(ss.size(), fs.size(), "size is same");

    auto ssi = ss.begin();
    auto fsi = fs.begin();

    while((ssi != ss.end()) && (fsi != fs.end())) {
        test.check_equal(*ssi, *fsi, "elements same");
        ++ssi;
        ++fsi;
        trck.passed_part(2);
    }

    test.check(ssi == ss.end(), "std::set iterator ok");
    test.check(fsi == fs.end(), "flat:set iterator ok");
}
//------------------------------------------------------------------------------
void flat_set_insert_at_end(auto& s) {
    eagitest::case_ test{s, "insert at end"};
    eagitest::track trck{test, 2, 2};
    auto& rg{test.random()};

    std::set<int> ss;
    eagine::flat_set<int> fs;

    for(unsigned i = 0; i < test.repeats(10000); ++i) {
        int k = rg.get_any<int>();
        ss.insert(ss.end(), k);
        fs.insert(fs.end(), k);
        trck.passed_part(1);
    }

    test.check_equal(ss.empty(), fs.empty(), "empty is same");
    test.check_equal(ss.size(), fs.size(), "size is same");

    auto ssi = ss.begin();
    auto fsi = fs.begin();

    while((ssi != ss.end()) && (fsi != fs.end())) {
        test.check_equal(*ssi, *fsi, "elements same");
        ++ssi;
        ++fsi;
        trck.passed_part(2);
    }

    test.check(ssi == ss.end(), "std::set iterator ok");
    test.check(fsi == fs.end(), "flat:set iterator ok");
}
//------------------------------------------------------------------------------
void flat_set_insert_at_lower_bound(auto& s) {
    eagitest::case_ test{s, "insert at lower bound"};
    eagitest::track trck{test, 2, 2};
    auto& rg{test.random()};

    std::set<int> ss;
    eagine::flat_set<int> fs;

    for(unsigned i = 0; i < test.repeats(10000); ++i) {
        int k = rg.get_any<int>();
        ss.insert(ss.lower_bound(k), k);
        fs.insert(fs.lower_bound(k), k);
        trck.passed_part(1);
    }

    test.check_equal(ss.empty(), fs.empty(), "empty is same");
    test.check_equal(ss.size(), fs.size(), "size is same");

    auto ssi = ss.begin();
    auto fsi = fs.begin();

    while((ssi != ss.end()) && (fsi != fs.end())) {
        test.check_equal(*ssi, *fsi, "elements same");
        ++ssi;
        ++fsi;
        trck.passed_part(2);
    }

    test.check(ssi == ss.end(), "std::set iterator ok");
    test.check(fsi == fs.end(), "flat:set iterator ok");
}
//------------------------------------------------------------------------------
void flat_set_erase(auto& s) {
    eagitest::case_ test{s, "erase"};
    eagitest::track trck{test, 3, 3};
    auto& rg{test.random()};

    std::set<int> ss;
    eagine::flat_set<int> fs;

    std::vector<int> ks;

    for(unsigned i = 0; i < test.repeats(10000); ++i) {
        int k = rg.get_any<int>();
        ks.push_back(k);

        ss.insert(k);
        fs.insert(k);
        trck.passed_part(1);
    }

    test.check_equal(ss.empty(), fs.empty(), "empty is same");
    test.check_equal(ss.size(), fs.size(), "size is same");

    for(int k : ks) {
        auto ssi = ss.find(k);
        auto fsi = fs.find(k);

        test.ensure(ssi != ss.end(), "std::set iterator ok");
        test.ensure(fsi != fs.end(), "flat_set iterator ok");
        test.check(fs.contains(k), "flat_set contains");

        test.check_equal(*ssi, *fsi, "elements are equal");

        const auto esmc = ss.erase(k);
        const auto efmc = fs.erase(k);

        test.check_equal(ss.size(), fs.size(), "size is same");
        test.check_equal(esmc, efmc, "erase count equal");
        trck.passed_part(2);
    }

    for(int k : ks) {
        auto ssi = ss.find(k);
        auto fsi = fs.find(k);

        test.check(ssi == ss.end(), "std::set iterator ok");
        test.check(fsi == fs.end(), "flat_set iterator ok");
        test.check(!fs.contains(k), "flat_set does not contain");
        trck.passed_part(3);
    }
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "flat_set"};
    test.once(flat_set_default_construct);
    test.once(flat_set_init_from_vector);
    test.once(flat_set_insert);
    test.once(flat_set_insert_at_begin);
    test.once(flat_set_insert_at_end);
    test.once(flat_set_insert_at_lower_bound);
    test.once(flat_set_erase);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
