/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.string:base64;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.valid_if;

namespace eagine {
//------------------------------------------------------------------------------
export [[nodiscard]] constexpr auto make_base64_encode_transform() {
    return [](const auto b) noexcept -> always_valid<char> {
        static const char table[66] =
          "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
        return table[std::min(std_size(b), std_size(64))];
    };
}
//------------------------------------------------------------------------------
export [[nodiscard]] constexpr auto make_base64_decode_transform() {
    return [](const char c) noexcept -> optionally_valid<byte> {
        static const auto table = []() {
            const char invtable[66] =
              "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
              "0123456789+/";
            std::array<byte, 256> result{};
            std::fill(result.begin(), result.end(), byte(0xFF));
            byte b{0};
            for(const auto x : invtable) {
                result[std_size(x)] = b++;
            }
            result[std_size('\0')] = byte(0xFF);
            return result;
        }();
        const auto r{table[std_size(c)]};
        return {r, (r & 0x80) != 0x80};
    };
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto base64_encoded_length(
  const span_size_t orig_size) noexcept {
    return dissolved_bits_length(orig_size, 6);
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto base64_decoded_length(
  const span_size_t orig_size) noexcept {
    return concentrated_bits_length(orig_size, 6);
}
//------------------------------------------------------------------------------
export template <typename Ps, typename Ss, typename Pd, typename Sd>
[[nodiscard]] auto base64_encode(
  const memory::basic_span<const byte, Ps, Ss> src,
  memory::basic_span<char, Pd, Sd> dst) -> memory::basic_span<char, Pd, Sd> {
    span_size_t i = 0;
    span_size_t o = 0;

    if(do_dissolve_bits(
         make_span_getter(i, src),
         make_span_putter(o, dst, make_base64_encode_transform()),
         6)) {
        return head(dst, o);
    }
    return {};
}
//------------------------------------------------------------------------------
export template <typename P, typename S, typename Dst>
[[nodiscard]] auto base64_encode(
  const memory::basic_span<const byte, P, S> src,
  Dst& dst) -> optional_reference<Dst> {
    using Ds = typename Dst::size_type;
    dst.resize(Ds(base64_encoded_length(src.size())));
    span_size_t i = 0;
    span_size_t o = 0;

    if(do_dissolve_bits(
         memory::make_span_getter(i, src),
         memory::make_span_putter(o, dst, make_base64_encode_transform()),
         6)) {
        dst.resize(Ds(o));
        return {dst};
    }
    return {nothing};
}
//------------------------------------------------------------------------------
export template <typename Ps, typename Ss, typename Pd, typename Sd>
[[nodiscard]] auto base64_decode(
  const memory::basic_span<const char, Ps, Ss> src,
  memory::basic_span<byte, Pd, Sd> dst) -> memory::basic_span<byte, Pd, Sd> {
    span_size_t i = 0;
    span_size_t o = 0;

    if(do_concentrate_bits(
         memory::make_span_getter(i, src, make_base64_decode_transform()),
         memory::make_span_putter(o, dst),
         6)) {
        return head(dst, o);
    }
    return {};
}
//------------------------------------------------------------------------------
export template <typename P, typename S, typename Dst>
[[nodiscard]] auto base64_decode(
  const memory::basic_span<const char, P, S> src,
  Dst& dst) -> optional_reference<Dst> {
    using Ds = typename Dst::size_type;
    dst.resize(Ds(base64_decoded_length(src.size())));
    span_size_t i = 0;
    span_size_t o = 0;

    if(do_concentrate_bits(
         memory::make_span_getter(i, src, make_base64_decode_transform()),
         memory::make_span_putter(o, dst),
         6)) {
        dst.resize(Ds(o));
        return {dst};
    }
    return {nothing};
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto assign_if_fits(
  string_view src,
  memory::buffer& dst,
  from_config_t) -> bool {
    return base64_decode(src, dst)
      .and_then([&](const auto& decoded) -> tribool {
          return decoded.empty() == src.empty();
      })
      .or_false();
}
//------------------------------------------------------------------------------
} // namespace eagine
