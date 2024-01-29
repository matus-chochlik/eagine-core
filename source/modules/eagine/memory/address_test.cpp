/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.memory;
//------------------------------------------------------------------------------
#ifdef CHECK
#undef CHECK
#endif
#define CHECK(X) check(X, #X)
//------------------------------------------------------------------------------
template <bool is_const>
void memory_address_1(eagitest::case_& test) {
    using namespace eagine;

    using cptr = const void*;
    using ptr = typename std::conditional<is_const, cptr, void*>::type;

    memory::basic_address<is_const> bma1;
    memory::basic_address<is_const> bma2(nullptr);
    memory::basic_address<is_const> bma3 = bma1;

    test.CHECK(not bool(bma1));
    test.CHECK(not bool(bma2));
    test.CHECK(not bool(bma3));
    test.CHECK(bool(not bma1));
    test.CHECK(bool(not bma2));
    test.CHECK(bool(not bma3));
    test.CHECK(static_cast<ptr>(bma1) == nullptr);
    test.CHECK(static_cast<ptr>(bma2) == nullptr);
    test.CHECK(static_cast<ptr>(bma3) == nullptr);
    test.CHECK(static_cast<cptr>(bma1) == nullptr);
    test.CHECK(static_cast<cptr>(bma2) == nullptr);
    test.CHECK(static_cast<cptr>(bma3) == nullptr);
    test.CHECK(bma1.get() == nullptr);
    test.CHECK(bma2.get() == nullptr);
    test.CHECK(bma3.get() == nullptr);
    test.check_equal(bma1.value(), 0, "value 1");
    test.check_equal(bma2.value(), 0, "value 2");
    test.check_equal(bma3.value(), 0, "value 3");
    test.check_equal(bma1 - bma2, 0, "difference 1");
    test.check_equal(bma2 - bma1, 0, "difference 2");
    test.check_equal(bma1 - bma3, 0, "difference 3");
    test.CHECK(bma1 == bma1);
    test.CHECK(bma2 == bma2);
    test.CHECK(bma3 == bma3);
    test.CHECK(bma1 == bma2);
    test.CHECK(bma1 == bma3);
    test.CHECK(bma2 == bma1);
    test.CHECK(not(bma1 != bma2));
    test.CHECK(not(bma1 != bma3));
    test.CHECK(not(bma2 != bma1));
    test.CHECK(not(bma1 != bma1));
    test.CHECK(not(bma2 != bma2));
    test.CHECK(not(bma3 != bma3));
    test.check_equal(bma1.misalignment(1), 0, "misalign 1");
    test.check_equal(bma1.misalignment(2), 0, "misalign 2");
    test.check_equal(bma1.misalignment(4), 0, "misalign 4");
    test.check_equal(bma1.misalignment(8), 0, "misalign 8");
    test.CHECK(bma2.is_aligned_to(1));
    test.CHECK(bma2.is_aligned_to(2));
    test.CHECK(bma2.is_aligned_to(4));
    test.CHECK(bma2.is_aligned_to(8));
}
//------------------------------------------------------------------------------
void address_1(auto& s) {
    eagitest::case_ test{s, 1, "1"};
    memory_address_1<true>(test);
    memory_address_1<false>(test);
}
//------------------------------------------------------------------------------
template <bool is_const>
void memory_address_2(eagitest::case_& test) {
    using namespace eagine;

    using cptr = const void*;
    using ptr = typename std::conditional<is_const, cptr, void*>::type;

    char c;
    int i;
    double d;

    memory::basic_address<is_const> bma1(&c);
    memory::basic_address<is_const> bma2(&i);
    memory::basic_address<is_const> bma3(&d);

    test.CHECK(bool(bma1));
    test.CHECK(bool(bma2));
    test.CHECK(bool(bma3));
    test.CHECK(!!bma1);
    test.CHECK(!!bma2);
    test.CHECK(!!bma3);
    test.CHECK(static_cast<ptr>(bma1) == &c);
    test.CHECK(static_cast<ptr>(bma2) == &i);
    test.CHECK(static_cast<ptr>(bma3) == &d);
    test.CHECK(static_cast<cptr>(bma1) == &c);
    test.CHECK(static_cast<cptr>(bma2) == &i);
    test.CHECK(static_cast<cptr>(bma3) == &d);
    test.check_equal(
      bma1.value(), reinterpret_cast<std::intptr_t>(&c), "value 1");
    test.check_equal(
      bma2.value(), reinterpret_cast<std::intptr_t>(&i), "value 2");
    test.check_equal(
      bma3.value(), reinterpret_cast<std::intptr_t>(&d), "value 3");
    test.check_equal(bma1 - bma2, bma1.ptr() - bma2.ptr(), "difference 1");
    test.check_equal(bma1 - bma3, bma1.ptr() - bma3.ptr(), "difference 2");
    test.check_equal(bma2 - bma3, bma2.ptr() - bma3.ptr(), "difference 3");
    test.CHECK(bma1 == bma1);
    test.CHECK(bma2 == bma2);
    test.CHECK(bma3 == bma3);
    test.CHECK(bma1 != bma2);
    test.CHECK(bma1 != bma3);
    test.CHECK(bma2 != bma3);
    test.CHECK(bma2 != bma1);
    test.CHECK(bma3 != bma1);
    test.CHECK(bma3 != bma2);
    test.check_equal(bma1.misalignment(alignof(char)), 0, "misalign 1");
    test.check_equal(bma2.misalignment(alignof(int)), 0, "misalign 2");
    test.check_equal(bma3.misalignment(alignof(double)), 0, "misalign 3");
    test.CHECK(bma1.is_aligned_to(alignof(char)));
    test.CHECK(bma2.is_aligned_to(alignof(int)));
    test.CHECK(bma3.is_aligned_to(alignof(double)));
}
//------------------------------------------------------------------------------
void address_2(auto& s) {
    eagitest::case_ test{s, 2, "2"};
    memory_address_2<true>(test);
    memory_address_2<false>(test);
}
#undef CHECK
//------------------------------------------------------------------------------
template <bool is_const>
void memory_address_3(eagitest::case_& test) {
    auto& rg{test.random()};
    using namespace eagine;

    for(unsigned i = 0; i < test.repeats(100); ++i) {
        int offs = rg.get_int(0, 1000) - rg.get_int(0, 1000);

        memory::basic_address<is_const> bma1(&i);
        memory::basic_address<is_const> bma2 = bma1 + offs;
        memory::basic_address<is_const> bma3 = bma1 - offs;

        test.check(bma1 == bma2 - offs, "offset ok 1");
        test.check(bma1 == bma3 + offs, "offset ok 2");
    }
}
//------------------------------------------------------------------------------
void address_3(auto& s) {
    eagitest::case_ test{s, 3, "3"};
    memory_address_3<true>(test);
    memory_address_3<false>(test);
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "address", 3};
    test.once(address_1);
    test.once(address_2);
    test.once(address_3);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
