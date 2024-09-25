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
template <typename Ident, bool useSimd>
auto ident_ops_t(auto& out, std::integral_constant<bool, useSimd>) noexcept {
    Ident id{};
    std::vector<Ident> vid;

    const std::size_t n = 128Z * 1024Z;
    vid.reserve(n);

    const time_measure measurement;

    const auto sort_pred{[](const Ident l, const Ident r) {
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
template <bool useSimd>
auto ident_ops(
  auto& out,
  std::integral_constant<bool, useSimd> use_simd = {}) noexcept {
    return ident_ops_t<
      basic_identifier<10, 6, default_identifier_char_set, identifier_t, useSimd>>(
      out, use_simd);
}
//------------------------------------------------------------------------------
template <bool useSimd>
auto id_v_ops(
  auto& out,
  std::integral_constant<bool, useSimd> use_simd = {}) noexcept {
    return ident_ops_t<basic_identifier_value<
      10,
      6,
      default_identifier_char_set,
      identifier_t,
      useSimd>>(out, use_simd);
}
//------------------------------------------------------------------------------
auto main(main_ctx& ctx) -> int {
    main_ctx_object out{"identifier", ctx};

    const auto t_simd_i{ident_ops(out, std::true_type{})};
    const auto t_base_i{ident_ops(out, std::false_type{})};

    out.cio_print("speedup (identifier): ${val}")
      .arg("val", t_simd_i / t_base_i);

    const auto t_simd_v{id_v_ops(out, std::true_type{})};
    const auto t_base_v{id_v_ops(out, std::false_type{})};

    out.cio_print("speedup (identifier_value): ${val}")
      .arg("val", t_simd_v / t_base_v);

    return 0;
}
//------------------------------------------------------------------------------
} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

