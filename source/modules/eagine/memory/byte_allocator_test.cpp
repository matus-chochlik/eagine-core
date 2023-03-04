/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.types;
import eagine.core.memory;
import std;
//------------------------------------------------------------------------------
template <typename T>
void memory_c_realloc_1_T(eagitest::case_& test, std::size_t n) {
    using namespace eagine;
    auto& rg{test.random()};

    memory::c_byte_reallocator a;

    const span_size_t ao = span_align_of<T>();
    const span_size_t sz = span_size_of<T>(n);

    test.check(a.max_size(ao) >= sz, "max size is ok");

    memory::owned_block b1 = a.allocate(sz, ao);

    test.check_equal(b1.empty(), n == 0, "empty");
    test.check(b1.size() >= sz, "size is ok 1");
    test.check(is_aligned_to(b1.addr(), ao), "is aligned to");

    test.check(!!a.has_allocated(b1, ao), "has allocated 1");

    memory::owned_block b2 = a.reallocate(std::move(b1), sz, ao);

    test.check(b1.empty(), "empty");
    test.check(b2.size() >= sz, "size is ok 2");

    a.deallocate(std::move(b2), ao);

    std::deque<memory::owned_block> blks;

    for(std::size_t i = 0; i < n; ++i) {
        blks.emplace_back(a.allocate(sizeof(T), ao));
    }

    test.check(blks.back().size() >= span_size_of<T>(), "size is ok 3");
    test.check(is_aligned_to(blks.back().addr(), ao), "is aligned to 2");

    for(memory::owned_block& blk : blks) {
        test.check(!!a.has_allocated(blk, ao), "has allocated 2");
    }

    while(not blks.empty()) {
        auto i = blks.begin() + rg.get_int(0, int(blks.size()) - 1);
        a.deallocate(std::move(*i), ao);
        blks.erase(i);
    }
}
//------------------------------------------------------------------------------
void c_byte_reallocator_1(unsigned i, auto& s) {
    eagitest::case_ test{s, 1, "C byte reallocator"};

    std::size_t f[2] = {0, 1};

    std::size_t n = f[(i + 0) % 2] + f[(i + 1) % 2];
    f[i % 2] = n;

    memory_c_realloc_1_T<char>(test, n);
    memory_c_realloc_1_T<short>(test, n);
    memory_c_realloc_1_T<int>(test, n);
    memory_c_realloc_1_T<float>(test, n);
    memory_c_realloc_1_T<double>(test, n);
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "byte_reallocator", 1};
    test.repeat(20, c_byte_reallocator_1);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
