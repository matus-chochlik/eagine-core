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
template <typename T>
void memory_std_alloc_1_T(eagitest::case_& test, std::size_t n) {
    using namespace eagine;

    memory::std_allocator<T> a;

    T* p = a.allocate(n);
    test.check(p != nullptr, "is not null");
    a.deallocate(p, n);

    std::vector<T, memory::std_allocator<T>> v;

    for(std::size_t i = 1; i <= n; ++i) {
        v.reserve(std_size(i));
    }

    for(std::size_t i = 1; i <= n; ++i) {
        v.push_back(T());
    }

    v.clear();

    for(std::size_t i = 1; i <= n; ++i) {
        v.push_back(T());
    }
    test.check_equal(v.std_size(), n, "size is ok");
}
//------------------------------------------------------------------------------
void std_byte_allocator_1(unsigned i, auto& s) {
    eagitest::case_ test{s, 1, "STL allocator"};

    std::size_t f[2] = {0, 1};

    std::size_t n = f[(i + 0) % 2] + f[(i + 1) % 2];
    f[i % 2] = n;

    memory_std_alloc_1_T<char>(test, n);
    memory_std_alloc_1_T<short>(test, n);
    memory_std_alloc_1_T<int>(test, n);
    memory_std_alloc_1_T<float>(test, n);
    memory_std_alloc_1_T<double>(test, n);
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "std_allocator", 1};
    test.repeat(100, std_byte_allocator_1);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
