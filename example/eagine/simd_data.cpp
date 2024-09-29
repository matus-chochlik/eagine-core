/// @example eagine/simd_data.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import std;

namespace eagine {
//------------------------------------------------------------------------------
template <bool useSimd>
auto vect_ops(auto& out, std::integral_constant<bool, useSimd> = {}) noexcept {

    const time_measure measurement;

    for(int i = 0; i < 100'000'000; ++i) {
        eagine::simd::data_t<int, 4, useSimd> va{
          eagine::simd::fill<int, 4, useSimd>::apply(i)};
        eagine::simd::data_t<int, 4, useSimd> vb{
          eagine::simd::fill<int, 4, useSimd>::apply(i)};

        if(not eagine::simd::vector_equal<int, 4, useSimd>::apply(va, vb)) {
            break;
        }
    }

    const auto seconds{measurement.seconds()};
    out.cio_print("time: ${sec}").arg("sec", seconds);

    return seconds;
}
//------------------------------------------------------------------------------
auto main(main_ctx& ctx) -> int {
    main_ctx_object out{"matrix", ctx};

    const auto t_simd{vect_ops(out, std::true_type{})};
    const auto t_base{vect_ops(out, std::false_type{})};

    out.cio_print("speedup: ${val}").arg("val", t_simd / t_base);

    return 0;
}
//------------------------------------------------------------------------------
} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

