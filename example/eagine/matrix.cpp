/// @example eagine/matrix.cpp
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
auto matrix_ops(auto& out, std::integral_constant<bool, useSimd> = {}) noexcept {

    using M = math::matrix<float, 4, 4, true, useSimd>;
    const M m0{
      {{1.F, 0.F, 0.F, 0.F},
       {0.F, 1.F, 0.F, 0.F},
       {0.F, 0.F, 1.F, 0.F},
       {0.F, 0.F, 0.F, 1.F}}};

    using V = math::vector<float, 4, useSimd>;
    const V v{1.F, 1.F, 1.F, 1.F};

    const time_measure measurement;

    for(int i = 0; i < 100'000; ++i) {
        const auto fi0{float(i)};
        const auto fi1{float(i + 1)};
        const auto fi2{float(i + 2)};
        const auto ang{radians_(fi0)};
        const M m{
          math::perspective<M>{-fi0, fi0, -fi0, fi0, 1.F, fi0} *
          math::rotation_x<M>{ang} * math::rotation_y<M>{ang} *
          math::rotation_z<M>{ang} * math::reflection_x<M>{true} *
          math::reflection_y<M>{true} * math::reflection_z<M>{true} *
          math::translation<M>{fi0, fi1, fi2} * math::scale<M>{fi0, fi0, fi0} *
          m0};
        V u{v};
        for(int j = 0; j < 1'000; ++j) {
            u = multiply(m, u);
        }
    }

    const auto seconds{measurement.seconds()};
    out.cio_print("time: ${sec}").arg("sec", seconds);

    return seconds;
}
//------------------------------------------------------------------------------
auto main(main_ctx& ctx) -> int {
    main_ctx_object out{"matrix", ctx};

    const auto t_simd{matrix_ops(out, std::true_type{})};
    const auto t_base{matrix_ops(out, std::false_type{})};

    out.cio_print("speedup: ${val}").arg("val", t_simd / t_base);

    return 0;
}
//------------------------------------------------------------------------------
} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

