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
import <deque>;
//------------------------------------------------------------------------------
// test 1
//------------------------------------------------------------------------------
template <typename T>
void memory_alloc_Tn_1(
  eagitest::case_& test,
  eagine::memory::block& b,
  std::size_t n) {
    eagitest::track trck{test, 0, 3};
    auto& rg{test.random()};
    using namespace eagine;

    memory::stack_byte_allocator a(b);

    const span_size_t ao = span_align_of<T>();
    const span_size_t sz = span_size_of<T>(n);

    test.check(a.max_size(ao) > 0, "max size");

    memory::owned_block b1 = a.allocate(sz, ao);

    test.check(not b1.empty(), "not empty");
    test.check(b1.size() >= sz, "size");
    test.check(is_aligned_to(b1.addr(), ao), "is aligned to");

    test.ensure(bool(a.has_allocated(b1, ao)), "has allocated");

    memory::owned_block b2 = a.reallocate(std::move(b1), sz, ao);

    test.ensure(bool(a.has_allocated(b2, ao)), "has allocated 2");

    a.deallocate(std::move(b2), ao);

    std::deque<memory::owned_block> blks;

    for(std::size_t i = 0; i < n; ++i) {
        blks.emplace_back(a.allocate(span_size_of<T>(), ao));
        trck.checkpoint(1);
    }

    for(memory::owned_block& blk : blks) {
        test.check(blks.back().size() >= span_size_of<T>(), "size");
        test.check(is_aligned_to(blks.back().addr(), ao), "is aligned to 2");
        test.check(bool(a.has_allocated(blk, ao)), "has allocated 3");
        trck.checkpoint(2);
    }

    while(not blks.empty()) {
        auto i = blks.begin() + rg.get_int(0, int(blks.size() - 1));
        a.deallocate(std::move(*i), ao);
        blks.erase(i);
        trck.checkpoint(3);
    }
}
//------------------------------------------------------------------------------
template <typename T>
void memory_alloc_T(eagitest::case_& test) {
    using namespace eagine;
    std::vector<char> buf;
    buf.resize(1024 * 1024);

    memory::block b = as_bytes(cover(buf));

    std::size_t fib[2] = {0, 1};

    for(unsigned i = 0; i < test.repeats(20); ++i) {
        std::size_t n = fib[(i + 0) % 2] + fib[(i + 1) % 2];
        fib[i % 2] = n;

        memory_alloc_Tn_1<T>(test, b, n);
    }
}
//------------------------------------------------------------------------------
void stack_allocator_char_1(auto& s) {
    eagitest::case_ test{s, 1, "char 1"};
    memory_alloc_T<char>(test);
}
//------------------------------------------------------------------------------
void stack_allocator_short_1(auto& s) {
    eagitest::case_ test{s, 2, "short 1"};
    memory_alloc_T<short>(test);
}
//------------------------------------------------------------------------------
void stack_allocator_int_1(auto& s) {
    eagitest::case_ test{s, 3, "int 1"};
    memory_alloc_T<int>(test);
}
//------------------------------------------------------------------------------
void stack_allocator_long_long_1(auto& s) {
    eagitest::case_ test{s, 4, "long long 1"};
    memory_alloc_T<long long>(test);
}
//------------------------------------------------------------------------------
void stack_allocator_float_1(auto& s) {
    eagitest::case_ test{s, 5, "float 1"};
    memory_alloc_T<float>(test);
}
//------------------------------------------------------------------------------
void stack_allocator_double_1(auto& s) {
    eagitest::case_ test{s, 6, "double 1"};
    memory_alloc_T<double>(test);
}
//------------------------------------------------------------------------------
// test 2
//------------------------------------------------------------------------------
template <typename T>
void memory_stack_alloc_T_hlp_2(
  std::deque<eagine::memory::owned_block>& blks,
  eagine::memory::stack_byte_allocator& a,
  std::size_t n) {
    blks.emplace_back(
      a.allocate(eagine::span_size_of<T>(n), eagine::span_align_of<T>()));
}
//------------------------------------------------------------------------------
void stack_allocator_2(auto& s) {
    eagitest::case_ test{s, 7, "2"};
    eagitest::track trck{test, 0, 3};
    auto& rg{test.random()};
    using namespace eagine;

    std::vector<char> buf;
    buf.resize(1024 * 1024);

    memory::stack_byte_allocator a(as_bytes(cover(buf)));

    std::deque<memory::owned_block> blks;

    for(unsigned i = 0; i < test.repeats(100); ++i) {
        const auto n = rg.get_std_size(0, 10);

        switch(i % 7) {
            case 0:
                memory_stack_alloc_T_hlp_2<int>(blks, a, n);
                break;
            case 1:
                memory_stack_alloc_T_hlp_2<long>(blks, a, n);
                break;
            case 2:
                memory_stack_alloc_T_hlp_2<float>(blks, a, n);
                break;
            case 3:
                memory_stack_alloc_T_hlp_2<double>(blks, a, n);
                break;
            case 4:
                memory_stack_alloc_T_hlp_2<short>(blks, a, n);
                break;
            default:
                memory_stack_alloc_T_hlp_2<char>(blks, a, n);
        }
        trck.checkpoint(1);
    }

    for(std::size_t n = blks.size(), i = 0; i < n; ++i) {
        if(not blks[i].empty()) {
            test.check(bool(a.has_allocated(blks[i], 0)), "has allocated");

            for(std::size_t j = i; j < n; ++j) {
                test.check(blks[i].overlaps(blks[j]) == (i == j), "overlaps");
                trck.checkpoint(2);
            }
        }
    }

    while(not blks.empty()) {
        auto i = blks.begin() + rg.get_int(0, int(blks.size() - 1));
        a.deallocate(std::move(*i), 0);
        blks.erase(i);
        trck.checkpoint(3);
    }
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "stack_allocator", 7};
    test.once(stack_allocator_char_1);
    test.once(stack_allocator_short_1);
    test.once(stack_allocator_int_1);
    test.once(stack_allocator_long_long_1);
    test.once(stack_allocator_float_1);
    test.once(stack_allocator_double_1);
    test.once(stack_allocator_2);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
