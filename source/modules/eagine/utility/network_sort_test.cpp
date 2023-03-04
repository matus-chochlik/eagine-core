/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.utility;
import eagine.core.memory;
import std;
//------------------------------------------------------------------------------
template <typename T, std::size_t N, typename Cmp, typename Case>
void network_sorter_1_TNC(Case& test, int n) {
    using namespace eagine;
    auto& rg{test.random()};

    for(int i = 0; i < n; ++i) {
        std::array<T, N> a;
        for(T& e : a) {
            e = rg.get_any(std::type_identity<T>{});
        }

        eagine::network_sorter<T, N, Cmp> s{a};
        const std::array<T, N>& r = s();

        test.check(std::is_sorted(r.begin(), r.end(), Cmp()), "is sorted 1");
    }

    for(int i = 0; i < n; ++i) {
        std::array<T, N> a;
        for(T& e : a) {
            e = rg.get_any(std::type_identity<T>{});
        }

        network_sort<N, Cmp>(cover(a));

        test.check(std::is_sorted(a.begin(), a.end(), Cmp()), "is sorted 2");
    }
}
//------------------------------------------------------------------------------
template <typename T, std::size_t N, typename Case>
void network_sorter_1_TN(Case& test, int n) {
    network_sorter_1_TNC<T, N, std::less<T>>(test, n);
    network_sorter_1_TNC<T, N, std::greater<T>>(test, n);
}
//------------------------------------------------------------------------------
template <std::size_t N, typename Case>
void network_sorter_1_N(Case& test, int n) {

    network_sorter_1_TN<int, N>(test, n);
    network_sorter_1_TN<float, N>(test, n);
    network_sorter_1_TN<double, N>(test, n);
    network_sorter_1_TN<unsigned, N>(test, n);
}
//------------------------------------------------------------------------------
void network_sort_1(auto& s) {
    eagitest::case_ test{s, 1, "1"};

    network_sorter_1_N<1>(test, test.repeats(100));
    network_sorter_1_N<2>(test, test.repeats(100));
    network_sorter_1_N<3>(test, test.repeats(100));
    network_sorter_1_N<4>(test, test.repeats(100));
    network_sorter_1_N<5>(test, test.repeats(100));
    network_sorter_1_N<6>(test, test.repeats(100));
    network_sorter_1_N<7>(test, test.repeats(90));
    network_sorter_1_N<8>(test, test.repeats(80));
    network_sorter_1_N<9>(test, test.repeats(70));
}
//------------------------------------------------------------------------------
void network_sort_2(auto& s) {
    eagitest::case_ test{s, 2, "2"};

    network_sorter_1_N<10>(test, test.repeats(70));
    network_sorter_1_N<11>(test, test.repeats(70));
    network_sorter_1_N<12>(test, test.repeats(70));
    network_sorter_1_N<14>(test, test.repeats(65));
    network_sorter_1_N<16>(test, test.repeats(60));
    network_sorter_1_N<32>(test, test.repeats(40));
    network_sorter_1_N<64>(test, test.repeats(20));
}
//------------------------------------------------------------------------------
void network_sort_3(auto& s) {
    eagitest::case_ test{s, 3, "3"};

    network_sorter_1_N<128>(test, test.repeats(10));
    network_sorter_1_N<256>(test, test.repeats(7));
    network_sorter_1_N<512>(test, test.repeats(4));
}
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "network_sort", 3};
    test.once(network_sort_1);
    test.once(network_sort_2);
    test.once(network_sort_3);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
