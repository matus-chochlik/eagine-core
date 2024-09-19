/// @example eagine/simd_ident.cpp
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
auto ident_ops(auto& out, std::integral_constant<bool, useSimd> = {}) noexcept {
    using ident_t =
      basic_identifier<10, 6, default_identifier_char_set, identifier_t, useSimd>;

    ident_t id{};
    std::vector<ident_t> vid;

    const std::size_t n = 128Z * 1024Z;
    vid.reserve(n);

    const time_measure measurement;

    const auto sort_pred{[](const ident_t l, const ident_t r) {
        return r < l;
    }};

    for(int i = 0; i < 100; ++i) {
        vid.clear();
        for(std::size_t k = 0Z; k < n; ++k) {
            vid.push_back(id);
            id = increment(id);
        }
        std::sort(vid.begin(), vid.end(), sort_pred);
        if(not std::is_sorted(vid.begin(), vid.end(), sort_pred)) {
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

    const auto t_simd{ident_ops(out, std::true_type{})};
    const auto t_base{ident_ops(out, std::false_type{})};

    out.cio_print("speedup: ${val}").arg("val", t_simd / t_base);

    return 0;
}
//------------------------------------------------------------------------------
} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

