/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.container;
import <map>;
import <functional>;
//------------------------------------------------------------------------------
void flat_map_init_from_vector(auto& s) {
    eagitest::case_ test{s, "init from vector"};
    eagitest::track trck{test, 2, 2};
    auto& rg{test.random()};

    std::map<int, std::size_t> sm;

    using p_t = std::pair<const int, std::size_t>;

    std::vector<p_t> d;

    std::hash<int> h;

    for(unsigned i = 0; i < test.repeats(10000); ++i) {
        int k = rg.get_any<int>();
        std::size_t v = h(k + k);

        sm.insert(p_t(k, v));
        d.push_back(p_t(k, v));
        trck.passed_part(1);
    }

    eagine::flat_map<int, std::size_t> fm(d);

    test.check_equal(sm.empty(), fm.empty(), "empty is same");
    test.check_equal(sm.size(), fm.size(), "size is same");

    auto smi = sm.begin();
    auto fmi = fm.begin();

    while((smi != sm.end()) && (fmi != fm.end())) {
        test.check_equal(smi->first, fmi->first, "first is same");
        test.check_equal(smi->second, fmi->second, "second is same");
        test.check_equal(fm[smi->first], smi->second, "fm[] ok");
        ++smi;
        ++fmi;
        trck.passed_part(2);
    }

    test.check(smi == sm.end(), "std::map iterator ok");
    test.check(fmi == fm.end(), "flat_map iterator ok");
}
//------------------------------------------------------------------------------
void flat_map_insert(auto& s) {
    eagitest::case_ test{s, "insert"};
    eagitest::track trck{test, 2, 2};
    auto& rg{test.random()};

    std::map<int, std::size_t> sm;
    eagine::flat_map<int, std::size_t> fm;

    using p_t = std::pair<const int, std::size_t>;

    std::hash<int> h;

    for(unsigned i = 0; i < test.repeats(10000); ++i) {
        int k = rg.get_any<int>();
        std::size_t v = h(k + k);

        sm.insert(p_t(k, v));
        fm.insert(p_t(k, v));
        trck.passed_part(1);
    }

    test.check_equal(sm.empty(), fm.empty(), "empty is same");
    test.check_equal(sm.size(), fm.size(), "size is same");

    auto smi = sm.begin();
    auto fmi = fm.begin();

    while((smi != sm.end()) && (fmi != fm.end())) {
        test.check_equal(smi->first, fmi->first, "first is same");
        test.check_equal(smi->second, fmi->second, "second is same");
        test.check_equal(fm[smi->first], smi->second, "fm[] is ok");
        ++smi;
        ++fmi;
        trck.passed_part(2);
    }

    test.check(smi == sm.end(), "std::map iterator ok");
    test.check(fmi == fm.end(), "flat_map iterator ok");
}
//------------------------------------------------------------------------------
void flat_map_insert_at_begin(auto& s) {
    eagitest::case_ test{s, "insert at begin"};
    eagitest::track trck{test, 2, 2};
    auto& rg{test.random()};

    std::map<int, std::size_t> sm;
    eagine::flat_map<int, std::size_t> fm;

    using p_t = std::pair<const int, std::size_t>;

    std::hash<int> h;

    for(unsigned i = 0; i < test.repeats(10000); ++i) {
        int k = rg.get_any<int>();
        std::size_t v = h(k + k);

        sm.insert(sm.begin(), p_t(k, v));
        fm.insert(fm.begin(), p_t(k, v));
        trck.passed_part(1);
    }

    test.check_equal(sm.empty(), fm.empty(), "empty is same");
    test.check_equal(sm.size(), fm.size(), "size is same");

    auto smi = sm.begin();
    auto fmi = fm.begin();

    while((smi != sm.end()) && (fmi != fm.end())) {
        test.check_equal(smi->first, fmi->first, "first is same");
        test.check_equal(smi->second, fmi->second, "second is same");
        test.check_equal(fm[smi->first], smi->second, "fm[] is ok");
        ++smi;
        ++fmi;
        trck.passed_part(2);
    }

    test.check(smi == sm.end(), "std::map iterator ok");
    test.check(fmi == fm.end(), "flat_map iterator ok");
}
//------------------------------------------------------------------------------
void flat_map_insert_at_end(auto& s) {
    eagitest::case_ test{s, "insert at end"};
    eagitest::track trck{test, 2, 2};
    auto& rg{test.random()};

    std::map<int, std::size_t> sm;
    eagine::flat_map<int, std::size_t> fm;

    using p_t = std::pair<const int, std::size_t>;

    std::hash<int> h;

    for(unsigned i = 0; i < test.repeats(10000); ++i) {
        int k = rg.get_any<int>();
        std::size_t v = h(k + k);

        sm.insert(sm.end(), p_t(k, v));
        fm.insert(fm.end(), p_t(k, v));
        trck.passed_part(1);
    }

    test.check_equal(sm.empty(), fm.empty(), "empty is same");
    test.check_equal(sm.size(), fm.size(), "size is same");

    auto smi = sm.begin();
    auto fmi = fm.begin();

    while((smi != sm.end()) && (fmi != fm.end())) {
        test.check_equal(smi->first, fmi->first, "first is same");
        test.check_equal(smi->second, fmi->second, "second is same");
        test.check_equal(fm[smi->first], smi->second, "fm[] is ok");
        ++smi;
        ++fmi;
        trck.passed_part(2);
    }

    test.check(smi == sm.end(), "std::map iterator ok");
    test.check(fmi == fm.end(), "flat_map iterator ok");
}
//------------------------------------------------------------------------------
void flat_map_insert_at_lower_bound(auto& s) {
    eagitest::case_ test{s, "insert at lower bound"};
    eagitest::track trck{test, 2, 2};
    auto& rg{test.random()};

    std::map<int, std::size_t> sm;
    eagine::flat_map<int, std::size_t> fm;

    using p_t = std::pair<const int, std::size_t>;

    std::hash<int> h;

    for(unsigned i = 0; i < test.repeats(10000); ++i) {
        int k = rg.get_any<int>();
        std::size_t v = h(k + k);

        sm.insert(sm.lower_bound(k), p_t(k, v));
        fm.insert(fm.lower_bound(k), p_t(k, v));
        trck.passed_part(1);
    }

    test.check_equal(sm.empty(), fm.empty(), "empty is same");
    test.check_equal(sm.size(), fm.size(), "size is same");

    auto smi = sm.begin();
    auto fmi = fm.begin();

    while((smi != sm.end()) && (fmi != fm.end())) {
        test.check_equal(smi->first, fmi->first, "first is same");
        test.check_equal(smi->second, fmi->second, "second is same");
        test.check_equal(fm[smi->first], smi->second, "fm[] is ok");
        ++smi;
        ++fmi;
        trck.passed_part(2);
    }

    test.check(smi == sm.end(), "std::map iterator ok");
    test.check(fmi == fm.end(), "flat_map iterator ok");
}
//------------------------------------------------------------------------------
void flat_map_try_emplace(auto& s) {
    eagitest::case_ test{s, "try emplace"};
    eagitest::track trck{test, 2, 2};
    auto& rg{test.random()};

    std::map<int, std::size_t> sm;
    eagine::flat_map<int, std::size_t> fm;

    std::hash<int> h;

    for(unsigned i = 0; i < test.repeats(10000); ++i) {
        int k = rg.get_any<int>();
        std::size_t v = h(k + k);

        const bool ism = sm.try_emplace(k, v).second;
        const bool ifm = fm.try_emplace(k, v).second;

        test.check_equal(ism, ifm, "emplace result");
        trck.passed_part(1);
    }

    test.check_equal(sm.empty(), fm.empty(), "empty is same");
    test.check_equal(sm.size(), fm.size(), "size is same");

    auto smi = sm.begin();
    auto fmi = fm.begin();

    while((smi != sm.end()) && (fmi != fm.end())) {
        test.check_equal(smi->first, fmi->first, "first is same");
        test.check_equal(smi->second, fmi->second, "second is same");
        test.check_equal(fm[smi->first], smi->second, "fm[] ok");
        ++smi;
        ++fmi;
        trck.passed_part(2);
    }

    test.check(smi == sm.end(), "std::map iterator ok");
    test.check(fmi == fm.end(), "flat_map iterator ok");
}
//------------------------------------------------------------------------------
void flat_map_emplace(auto& s) {
    eagitest::case_ test{s, "emplace"};
    eagitest::track trck{test, 2, 2};
    auto& rg{test.random()};

    std::map<int, std::size_t> sm;
    eagine::flat_map<int, std::size_t> fm;

    std::hash<int> h;

    for(unsigned i = 0; i < test.repeats(10000); ++i) {
        int k = rg.get_any<int>();
        std::size_t v = h(k + k);

        const bool ism = sm.emplace(k, v).second;
        const bool ifm = fm.emplace(k, v).second;

        test.check_equal(ism, ifm, "emplace result");
        trck.passed_part(1);
    }

    test.check_equal(sm.empty(), fm.empty(), "empty is same");
    test.check_equal(sm.size(), fm.size(), "size is same");

    auto smi = sm.begin();
    auto fmi = fm.begin();

    while((smi != sm.end()) && (fmi != fm.end())) {
        test.check_equal(smi->first, fmi->first, "first is same");
        test.check_equal(smi->second, fmi->second, "second is same");
        test.check_equal(fm[smi->first], smi->second, "fm[] ok");
        ++smi;
        ++fmi;
        trck.passed_part(2);
    }

    test.check(smi == sm.end(), "std::map iterator ok");
    test.check(fmi == fm.end(), "flat_map iterator ok");
}
//------------------------------------------------------------------------------
void flat_map_key_assign(auto& s) {
    eagitest::case_ test{s, "assign by key"};
    eagitest::track trck{test, 2, 2};
    auto& rg{test.random()};

    std::map<int, std::size_t> sm;
    eagine::flat_map<int, std::size_t> fm;

    std::hash<int> h;

    for(unsigned i = 0; i < test.repeats(10000); ++i) {
        int k = rg.get_any<int>();
        std::size_t v = h(k + k);

        sm.emplace(k, v);
        fm[k] = v;
        trck.passed_part(1);
    }

    test.check_equal(sm.empty(), fm.empty(), "empty is same");
    test.check_equal(sm.size(), fm.size(), "size is same");

    auto smi = sm.begin();

    while(smi != sm.end()) {
        test.check_equal(fm[smi->first], smi->second, "fm[] ok");
        ++smi;
        trck.passed_part(2);
    }
}
//------------------------------------------------------------------------------
void flat_map_erase_key(auto& s) {
    eagitest::case_ test{s, "erase key"};
    auto& rg{test.random()};
    eagitest::track trck{test, 3, 3};

    std::map<int, std::size_t> sm;
    eagine::flat_map<int, std::size_t> fm;

    using p_t = std::pair<const int, std::size_t>;
    std::vector<int> ks;

    std::hash<int> h;

    for(unsigned i = 0; i < test.repeats(10000); ++i) {
        int k = rg.get_any<int>();
        std::size_t v = h(k + k);
        ks.push_back(k);

        sm.insert(p_t(k, v));
        fm.insert(p_t(k, v));
        trck.passed_part(1);
    }

    test.check_equal(sm.empty(), fm.empty(), "empty is same");
    test.check_equal(sm.size(), fm.size(), "size is same");

    for(int k : ks) {
        auto smi = sm.find(k);
        auto fmi = fm.find(k);

        test.ensure(smi != sm.end(), "std::map iterator ok");
        test.ensure(fmi != fm.end(), "flat_map iterator ok");

        test.check_equal(smi->first, fmi->first, "first is same");
        test.check_equal(smi->second, fmi->second, "second is same");
        test.check_equal(fm[smi->first], smi->second, "fm[] ok");

        const auto esmc = sm.erase(k);
        const auto efmc = fm.erase(k);

        test.check_equal(sm.size(), fm.size(), "same erased size");
        test.check_equal(esmc, efmc, "same erased count");
        trck.passed_part(2);
    }

    for(int k : ks) {
        auto smi = sm.find(k);
        auto fmi = fm.find(k);

        test.check(smi == sm.end(), "std::map key not found");
        test.check(fmi == fm.end(), "flat_map key not found");
        trck.passed_part(3);
    }
}
//------------------------------------------------------------------------------
void flat_map_erase_if(auto& s) {
    eagitest::case_ test{s, "erase if"};
    eagitest::track trck{test, 2, 2};
    auto& rg{test.random()};

    eagine::flat_map<int, std::size_t> fm;

    std::hash<int> h;

    for(unsigned i = 0; i < test.repeats(10000); ++i) {
        int k = rg.get_any<int>();
        std::size_t v = h(k + k);

        fm.emplace(k, v);
        trck.passed_part(1);
    }
    const auto is_odd = [](auto& p) {
        return p.second % 2 != 0;
    };
    fm.erase_if(is_odd);

    for(auto& p : fm) {
        test.check_equal(p.second % 2, 0U, "all are even");
        trck.passed_part(2);
    }
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "flat_map"};
    test.once(flat_map_init_from_vector);
    test.once(flat_map_insert);
    test.once(flat_map_insert_at_begin);
    test.once(flat_map_insert_at_end);
    test.once(flat_map_insert_at_lower_bound);
    test.once(flat_map_try_emplace);
    test.once(flat_map_emplace);
    test.once(flat_map_key_assign);
    test.once(flat_map_erase_key);
    test.once(flat_map_erase_if);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
