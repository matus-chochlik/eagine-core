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
void memory_alloc_1_T(eagitest::case_& test) {
    auto& rg{test.random()};
    using namespace eagine;

    memory::shared_byte_allocator a;

    const span_size_t ao = span_align_of<T>();

    test.check(not bool(a), "is not true");
    test.check(not a, "is false");

    test.check(not(a.max_size(ao) > 0), "max_size");

    memory::owned_block b = a.allocate(0, ao);

    test.check(not bool(b), "is not true");
    test.check(not b, "is false");

    test.check(!!a.has_allocated(b, ao), "not not has allocated");

    test.check(a.can_reallocate(b, 0, ao), "can reallocate");
    test.check(
      not a.can_reallocate(b, rg.get_span_size(1, 1000), ao),
      "cannot reallocate");

    b = a.reallocate(std::move(b), 0, ao);

    test.check(not bool(b), "is not true");
    test.check(not b, "is false");

    a.deallocate(std::move(b), ao);
}
//------------------------------------------------------------------------------
void shared_allocator_char_1(auto& s) {
    eagitest::case_ test{s, 1, "char 1"};
    memory_alloc_1_T<char>(test);
}
//------------------------------------------------------------------------------
void shared_allocator_short_1(auto& s) {
    eagitest::case_ test{s, 2, "short 1"};
    memory_alloc_1_T<short>(test);
}
//------------------------------------------------------------------------------
void shared_allocator_int_1(auto& s) {
    eagitest::case_ test{s, 3, "int 1"};
    memory_alloc_1_T<int>(test);
}
//------------------------------------------------------------------------------
void shared_allocator_long_long_1(auto& s) {
    eagitest::case_ test{s, 4, "long long 1"};
    memory_alloc_1_T<long long>(test);
}
//------------------------------------------------------------------------------
void shared_allocator_float_1(auto& s) {
    eagitest::case_ test{s, 5, "float 1"};
    memory_alloc_1_T<float>(test);
}
//------------------------------------------------------------------------------
void shared_allocator_double_1(auto& s) {
    eagitest::case_ test{s, 6, "double 1"};
    memory_alloc_1_T<double>(test);
}
//------------------------------------------------------------------------------
// test 2
//------------------------------------------------------------------------------
template <typename T>
void memory_alloc_2_Tn(eagitest::case_& test, std::size_t n) {
    using namespace eagine;

    memory::shared_byte_allocator a{memory::c_byte_reallocator()};

    const span_size_t ao = span_align_of<T>();
    const span_size_t sz = span_size_of<T>(n);

    test.check(bool(a), "a is true");
    test.check(!!a, "a not not false");

    test.check(a.max_size(ao) > 0, "max_size");

    memory::owned_block b = a.allocate(sz, ao);

    test.check(bool(a), "a is true");
    test.check(!!a, "a not not false");

    test.check(!!a.has_allocated(b, ao), "not not has allocated");

    test.check(a.can_reallocate(b, sz, ao), "can reallocate");

    b = a.reallocate(std::move(b), sz, ao);

    test.check(bool(b), "b is true");
    test.check(!!b, "b not not false");

    test.check(a.can_reallocate(b, sz * 2, ao), "can reallocate");

    b = a.reallocate(std::move(b), sz * 2, ao);

    test.check(bool(b), "b is true");
    test.check(!!b, "b not not false");

    test.check(a.can_reallocate(b, sz, ao), "can reallocate");

    b = a.reallocate(std::move(b), sz, ao);

    test.check(bool(b), "b is true");
    test.check(!!b, "not b not false");

    a.deallocate(std::move(b), ao);

    test.check(not bool(b), "b is false");
    test.check(not b, "b not true");
}
//------------------------------------------------------------------------------
template <typename T>
void memory_alloc_2_T(eagitest::case_& test) {
    std::size_t fib[2] = {0, 1};

    for(unsigned i = 0; i < test.repeats(30); ++i) {
        std::size_t n = fib[(i + 0) % 2] + fib[(i + 1) % 2];
        fib[i % 2] = n;

        memory_alloc_2_Tn<char>(test, n);
        memory_alloc_2_Tn<short>(test, n);
        memory_alloc_2_Tn<int>(test, n);
        memory_alloc_2_Tn<long>(test, n);
        memory_alloc_2_Tn<double>(test, n);
    }
}
//------------------------------------------------------------------------------
void shared_allocator_char_2(auto& s) {
    eagitest::case_ test{s, 7, "char 2"};
    memory_alloc_2_T<char>(test);
}
//------------------------------------------------------------------------------
void shared_allocator_short_2(auto& s) {
    eagitest::case_ test{s, 8, "short 2"};
    memory_alloc_2_T<short>(test);
}
//------------------------------------------------------------------------------
void shared_allocator_int_2(auto& s) {
    eagitest::case_ test{s, 9, "int 2"};
    memory_alloc_2_T<int>(test);
}
//------------------------------------------------------------------------------
void shared_allocator_long_long_2(auto& s) {
    eagitest::case_ test{s, 10, "long long 2"};
    memory_alloc_2_T<long long>(test);
}
//------------------------------------------------------------------------------
void shared_allocator_float_2(auto& s) {
    eagitest::case_ test{s, 11, "float 2"};
    memory_alloc_2_T<float>(test);
}
//------------------------------------------------------------------------------
void shared_allocator_double_2(auto& s) {
    eagitest::case_ test{s, 12, "double 2"};
    memory_alloc_2_T<double>(test);
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "shared_allocator", 12};
    test.once(shared_allocator_char_1);
    test.once(shared_allocator_short_1);
    test.once(shared_allocator_int_1);
    test.once(shared_allocator_long_long_1);
    test.once(shared_allocator_float_1);
    test.once(shared_allocator_double_1);
    test.once(shared_allocator_char_2);
    test.once(shared_allocator_short_2);
    test.once(shared_allocator_int_2);
    test.once(shared_allocator_long_long_2);
    test.once(shared_allocator_float_2);
    test.once(shared_allocator_double_2);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
